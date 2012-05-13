#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include <QObject>
#include <QFile>
#include <qextserialport.h>

typedef enum StartStatus
{
    ProgrammerInitializing,
    ProgrammerInitialized
} StartStatus;

typedef enum ReadStatus
{
    ReadStarting,
    ReadComplete,
    ReadError,
    ReadCancelled,
    ReadTimedOut
} ReadStatus;

typedef enum WriteStatus
{
    WriteErasing,
    WriteComplete,
    WriteError,
    WriteCancelled,
    WriteEraseComplete,
    WriteEraseFailed,
    WriteTimedOut
} WriteStatus;

typedef enum ElectricalTestStatus
{
    ElectricalTestStarted,
    ElectricalTestPassed,
    ElectricalTestFailed,
    ElectricalTestTimedOut,
    ElectricalTestCouldntStart
} ElectricalTestStatus;

typedef enum IdentificationStatus
{
    IdentificationStarting,
    IdentificationComplete,
    IdentificationError,
    IdentificationTimedOut
} IdentificationStatus;

typedef enum FirmwareFlashStatus
{
    FirmwareFlashStarting,
    FirmwareFlashComplete,
    FirmwareFlashError,
    FirmwareFlashCancelled,
    FirmwareFlashTimedOut
} FirmwareFlashStatus;

// Electrical test indexes
#define GROUND_FAIL_INDEX					0xFF

#define FIRST_ADDRESS_LINE_FAIL_INDEX		0
#define LAST_ADDRESS_LINE_FAIL_INDEX		(FIRST_ADDRESS_LINE_FAIL_INDEX + 20)
#define FIRST_DATA_LINE_FAIL_INDEX			(LAST_ADDRESS_LINE_FAIL_INDEX + 1)
#define LAST_DATA_LINE_FAIL_INDEX			(FIRST_DATA_LINE_FAIL_INDEX + 31)
#define CS_FAIL_INDEX						(LAST_DATA_LINE_FAIL_INDEX + 1)
#define OE_FAIL_INDEX						(CS_FAIL_INDEX + 1)
#define WE_FAIL_INDEX						(OE_FAIL_INDEX + 1)

class Programmer : public QObject
{
    Q_OBJECT
public:
    explicit Programmer(QObject *parent = 0);
    virtual ~Programmer();
    void ReadSIMMToFile(QString filename);
    void WriteFileToSIMM(QString filename);
    void RunElectricalTest();
    QString electricalTestPinName(uint8_t index);
    void IdentifySIMMChips();
    void GetChipIdentity(int chipIndex, uint8_t *manufacturer, uint8_t *device);
    void FlashFirmware(QString filename);
signals:
    void startStatusChanged(StartStatus status);

    void readStatusChanged(ReadStatus status);
    void readTotalLengthChanged(uint32_t total);
    void readCompletionLengthChanged(uint32_t total);

    void writeStatusChanged(WriteStatus status);
    void writeTotalLengthChanged(uint32_t total);
    void writeCompletionLengthChanged(uint32_t len);

    void electricalTestStatusChanged(ElectricalTestStatus status);
    void electricalTestFailLocation(uint8_t loc1, uint8_t loc2);

    void identificationStatusChanged(IdentificationStatus status);

    void firmwareFlashStatusChanged(FirmwareFlashStatus status);
    void firmwareFlashTotalLengthChanged(uint32_t total);
    void firmwareFlashCompletionLengthChanged(uint32_t total);
public slots:

private:
    QFile *readFile;
    QFile *writeFile;
    QFile *firmwareFile;

    QextSerialPort *serialPort;
    void sendByte(uint8_t b);
    uint8_t readByte();
    void handleChar(uint8_t c);

    uint32_t writeLenRemaining;
    uint32_t lenWritten;
    uint32_t electricalTestErrorCounter;
    uint8_t electricalTestFirstErrorLoc;

    uint32_t readChunkLenRemaining;
    uint32_t lenRead;

    int identificationCounter;
    uint8_t chipManufacturerIDs[4];
    uint8_t chipDeviceIDs[4];

    uint32_t firmwareLenRemaining;
    uint32_t firmwareLenWritten;

    void startProgrammerCommand(uint8_t commandByte, uint32_t newState);
    void startBootloaderCommand(uint8_t commandByte, uint32_t newState);

private slots:
    void dataReady();
};

#endif // PROGRAMMER_H
