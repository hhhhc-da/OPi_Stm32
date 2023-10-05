#include "MyProject.h"

using namespace std;

/*
 * OP_BASE          bit2    OPi-8
 * OP_BASE + 1      bit1    OPi-9
 * OP_BASE + 2      bit0    OPi-10
 * OP_SYNC          SYNC    OPi-12
 */

#define OP_BASE 8
#define OP_SYNC 12

const char *File_Name = "Data.xls";

int main()
{
    // Initialization Function
    int fd;

    if ((fd = serialOpen("/dev/ttyS1", 115200)) < 0)
    {
        cerr << "Unable to open serial!" << endl;
        return 1;
    }

    if (wiringPiSetup() == -1)
    {
        cerr << "GPIO initialization fail in line:" << __LINE__ << endl;
        return 1;
    }

    cout << "UART & GPIO setup OK" << endl;

    GPIO_Init();

    outputModeSet(READ_NONE);

    // Main Logical Function
    libxl::Book *book;

    while (true)
    {
        book = xlCreateBook();
        delay(200);

        if (book)
            break;
    }

    cout << "Excel prepare ready..." << endl;

    libxl::Sheet *sheet = book->addSheet("Data");

    if (sheet)
    {
        sheet->writeStr(1, 1, "Weight");
        sheet->writeStr(2, 1, "MQxDanger");
        sheet->writeStr(3, 1, "Temperature");
        sheet->writeStr(4, 1, "Humidity");
        sheet->writeStr(5, 1, "Distance");
    }

    book->save(File_Name);

    cout << "Excel initialization OK..." << endl;

    int TmpData = 0, TmpData2 = 0;
    double TmpDataDouble = 0.0;

    vector<char> receiver;
    string received_str;

    size_t epoch = 1;

    // Clear Serial Data
    while (serialDataAvail(fd))
        serialGetchar(fd);

    cout << "Reset Serial OK..." << endl;

    while (true)
    {
        try
        {
            outputModeSet(READ_NONE);

            while (serialDataAvail(fd))
                serialGetchar(fd);

            cout << "\nEpoch " + to_string(epoch++) << endl;

            delay(200);

            outputModeSet(READ_HUMI);

            // wait data
            while (!serialDataAvail(fd))
                ;

            // get all data
            while (serialDataAvail(fd))
            {
                receiver.push_back(static_cast<char>(serialGetchar(fd)));
            }

            received_str = string(receiver.begin(), receiver.end());

            TmpData = stoi(received_str);

            receiver.clear();

            cout << "HUMI:" + to_string(TmpData) << endl;

            sheet->writeNum(4, 2, TmpData);

            while (serialDataAvail(fd))
                serialGetchar(fd);

            delay(200);

            outputModeSet(READ_TEMP);

            while (!serialDataAvail(fd))
                ;

            while (serialDataAvail(fd))
            {
                receiver.push_back(static_cast<char>(serialGetchar(fd)));
            }
            received_str = string(receiver.begin(), receiver.end());
            TmpData = stoi(received_str);

            receiver.clear();

            cout << "TEMP:" + to_string(TmpData) << endl;

            sheet->writeNum(3, 2, TmpData);

            while (serialDataAvail(fd))
                serialGetchar(fd);

            delay(200);

            outputModeSet(READ_MQX);

            while (!serialDataAvail(fd))
                ;

            while (serialDataAvail(fd))
            {
                receiver.push_back(static_cast<char>(serialGetchar(fd)));
            }
            received_str = string(receiver.begin(), receiver.end());
            TmpData = stoi(received_str);

            receiver.clear();

            cout << "MQX:" + to_string(TmpData) << endl;

            sheet->writeNum(2, 2, TmpData);

            while (serialDataAvail(fd))
                serialGetchar(fd);

            delay(200);

            outputModeSet(READ_WEIGHT_HIGH);

            while (!serialDataAvail(fd))
                ;

            while (serialDataAvail(fd))
            {
                receiver.push_back(static_cast<char>(serialGetchar(fd)));
            }
            received_str = string(receiver.begin(), receiver.end());
            TmpData = stoi(received_str);

            receiver.clear();

            while (serialDataAvail(fd))
                serialGetchar(fd);

            delay(200);

            outputModeSet(READ_WEIGHT_LOW);

            while (!serialDataAvail(fd))
                ;

            while (serialDataAvail(fd))
            {
                receiver.push_back(static_cast<char>(serialGetchar(fd)));
            }
            received_str = string(receiver.begin(), receiver.end());
            TmpData2 = stoi(received_str);

            receiver.clear();

            while (serialDataAvail(fd))
                serialGetchar(fd);

            TmpDataDouble = TmpData + TmpData2 / 100.0;
            cout << "WEIGHT:" + to_string(TmpDataDouble) << endl;

            sheet->writeNum(1, 2, TmpDataDouble);

            delay(200);

            outputModeSet(READ_LENGTH_HIGH);

            while (!serialDataAvail(fd))
                ;

            while (serialDataAvail(fd))
            {
                receiver.push_back(static_cast<char>(serialGetchar(fd)));
            }
            received_str = string(receiver.begin(), receiver.end());
            TmpData = stoi(received_str);

            receiver.clear();

            while (serialDataAvail(fd))
                serialGetchar(fd);

            delay(200);

            outputModeSet(READ_LENGTH_LOW);

            while (!serialDataAvail(fd))
                ;

            while (serialDataAvail(fd))
            {
                receiver.push_back(static_cast<char>(serialGetchar(fd)));
            }
            received_str = string(receiver.begin(), receiver.end());
            TmpData2 = stoi(received_str);

            receiver.clear();

            while (serialDataAvail(fd))
                serialGetchar(fd);

            TmpDataDouble = TmpData + TmpData2 / 1000000.0;
            cout << "DISTANCE:" + to_string(TmpDataDouble) << endl;

            sheet->writeNum(5, 2, TmpDataDouble);

            book->save(File_Name);
            cout << "Write Excel File Done." << endl;
        }
        catch (exception e)
        {
            cout << e.what() << endl;
        }
        catch (...)
        {
            cout << "A little error ocurred.Retry." << endl;
        }
    }

    // Program Shouldn't Jump Into
    while (true)
    {
        cerr << "Fatal Error: drop in infinite loop ('MAIN' Function)" << endl;
        delay(2000);
    }

    return 1;
}

uint8_t GPIO_Init(void)
{
    pinMode(OP_BASE, OUTPUT);
    digitalWrite(OP_BASE, 0);

    pinMode(OP_BASE + 1, OUTPUT);
    digitalWrite(OP_BASE + 1, 0);

    pinMode(OP_BASE + 2, OUTPUT);
    digitalWrite(OP_BASE + 2, 0);

    pinMode(OP_SYNC, INPUT);
    pullUpDnControl(OP_SYNC, PUD_OFF);

    return (uint8_t)OK;
}

uint8_t outputModeSet(Read_Type type)
{
    switch (type)
    {
    case READ_NONE:
        digitalWrite(OP_BASE, 0);
        digitalWrite(OP_BASE + 1, 0);
        digitalWrite(OP_BASE + 2, 0);
        break;

    case READ_HUMI:
        digitalWrite(OP_BASE, 0);
        digitalWrite(OP_BASE + 1, 0);
        digitalWrite(OP_BASE + 2, 1);
        break;

    case READ_TEMP:
        digitalWrite(OP_BASE, 0);
        digitalWrite(OP_BASE + 1, 1);
        digitalWrite(OP_BASE + 2, 0);
        break;

    case READ_MQX:
        digitalWrite(OP_BASE, 0);
        digitalWrite(OP_BASE + 1, 1);
        digitalWrite(OP_BASE + 2, 1);
        break;

    case READ_LENGTH_HIGH:
        digitalWrite(OP_BASE, 1);
        digitalWrite(OP_BASE + 1, 0);
        digitalWrite(OP_BASE + 2, 0);
        break;

    case READ_LENGTH_LOW:
        digitalWrite(OP_BASE, 1);
        digitalWrite(OP_BASE + 1, 0);
        digitalWrite(OP_BASE + 2, 1);
        break;

    case READ_WEIGHT_HIGH:
        digitalWrite(OP_BASE, 1);
        digitalWrite(OP_BASE + 1, 1);
        digitalWrite(OP_BASE + 2, 0);
        break;

    case READ_WEIGHT_LOW:
        digitalWrite(OP_BASE, 1);
        digitalWrite(OP_BASE + 1, 1);
        digitalWrite(OP_BASE + 2, 1);
        break;

    default:
        cerr << "Input Type Error.\nUse 'READ_NONE' as default." << endl;

        digitalWrite(OP_BASE, 0);
        digitalWrite(OP_BASE + 1, 0);
        digitalWrite(OP_BASE + 2, 0);
        break;
    }

    return (uint8_t)OK;
}
