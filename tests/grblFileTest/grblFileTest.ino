// A program to test re-entrancy issues associated with
// the ESP32 SD and SPIFFs file systems and a wrapper
// around them to protect against such re-entrancy issues.

// Press Z from serial port (in loop) to re-initialize

#include "grblDebug.h"

// #define WITH_UI

#ifdef WITH_UI
    #include <grblTouchUI.h>
    #include <uiDirectory.h>

    uiDirectory *sp_uiDirectory = 0;

        // requires protectedFS
#endif


#define GRBL_FILE_OK     0
#define GRBL_FILE_ERROR -1
#define GRBL_FILE_EOF   -2

#define TEST_SDCARD             // otherwise will test SPIFFS
#define USE_PROTECTED_FS        // otherwise will directly use FS


#include <FS.h>
#ifdef TEST_SDCARD
    #include <SD.h>
    #include <SPI.h>
    #define FILL_BYTES 10000
#else
    #include <SPIFFS.h>
    #define FILL_BYTES 10
#endif


#ifdef USE_PROTECTED_FS
    #include <protectedFS.h>
    #define fileType proFile
    #ifdef TEST_SDCARD
        #define fileSystem sdCard
        protectedFS sdCard(SD);
    #else
        #define fileSystem spiFFS
        protectedFS spiFFS(SPIFFS);
    #endif
#else
    #define fileType File
    #ifdef TEST_SDCARD
        #define fileSystem SD
    #else
        #define fileSystem SPIFFS
    #endif
#endif




#define GRBL_SDCARD_CS    GPIO_NUM_21
void listDir(const char *dir, bool recursive=false,int level=0);



//-------------------------------------------
// grbl glue
//-------------------------------------------
// outside of grbl we implement gdbg_send() and gdbg_sendf()

void gdbg_send(const char* text)
{
    Serial.println(text);
}


void gdbg_sendf(const char* format, ...)
{
    char    loc_buf[64];
    char*   temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    size_t len = vsnprintf(NULL, 0, format, arg);
    va_end(copy);
    if (len >= sizeof(loc_buf))
    {
        temp = new char[len + 1];
        if (temp == NULL)
        {
            return;
        }
    }
    len = vsnprintf(temp, len + 1, format, arg);
    Serial.println(temp);
    va_end(arg);
    if (temp != loc_buf)
    {
        delete[] temp;
    }
}


void gdbgUint64(const char *format, uint64_t bytes)
{
    float div =
        bytes >= (1024 * 1024 * 1024) ? (1024 * 1024 * 1024) :
        bytes >= (1024 * 1024) ? (1024 * 1024) :
        bytes >= 1024 ? 1024 : 1;

    const char *div_size =
        bytes >= (1024 * 1024 * 1024) ? "GB" :
        bytes >= (1024 * 1024) ? "MB" :
        bytes >= 1024 ? "kb" : "bytes";

    float size = ((float)bytes)/div;
    gdbg_sendf(format,size,div_size);
}



//------------------------------------
// setup()
//------------------------------------

void setup()
{
    Serial.begin(115200);
    gdbg_send("\nfileTypeTest setup() started");

    #if 0   // def TEST_SDCARD
        // Put a volt meter on MISO ... it needs to be 3.3V here!
        //
        // The SD card should be inited before any other SPI devices
        // because it is not a well behaved SPI device until it is inited!
        // Besides the mandatory pullup (10K) on MISO, it seems to help if
        // we make sure the other CS pins are driven high (they could have
        // pullups too). This code forces them high before calling SD.begin(),
        // but is not appropriate in the fileTypeSystem object itself.

        // Serial.println("setting pins high");
        pinMode(GPIO_NUM_5,OUTPUT);         // TOUCH_CS
        pinMode(GPIO_NUM_17,OUTPUT);        // TFT_CS
        // pinMode(GPIO_NUM_19,INPUT_PULLUP);  // MISO
        digitalWrite(GPIO_NUM_5,1);
        digitalWrite(GPIO_NUM_17,1);
        vTaskDelay(250);
        // Serial.println("continuing");
    #endif


    if (initFileSystem())
    {
        #ifdef WITH_UI
            gdbg_send("initializing touchUI");
            touchUI_init();
        #endif

        listDir("/",false);

        gdbg_send("\ncommands:");
        gdbg_send("    z == remove test data");
        gdbg_send("    c == create test data");
        gdbg_send("    L == list test data");
        gdbg_send("    l == list /");
        gdbg_send("    t == run one time tests");
        #ifdef TEST_SDCARD
            gdbg_send("    T == loop test for SD card removal");
        #endif

        gdbg_send("");

    }
    else
        gdbg_send("fileTypeTest initFileSystem() failed");

    gdbg_send("fileTypeTest setup() completed");
}



bool initFileSystem()
{
    #ifdef TEST_SDCARD
        #define DEFAULT_FREQUENCY    4000000
        #define FAST_FREQUENCY       40000000

        if (!SD.begin(GRBL_SDCARD_CS,SPI,FAST_FREQUENCY))
        {
            gdbg_send("NO SD CARD");
            return false;
        }
    #else
        SPIFFS.begin(true);
    #endif

    showFSStats();
}


void showFSStats()
{
    #ifdef TEST_SDCARD
        uint8_t cardType = SD.cardType();
        gdbg_sendf("SD Card Type: %s",
            cardType == CARD_NONE ? "NONE" :
            cardType == CARD_MMC  ? "MMC" :
            cardType == CARD_SD   ? "SD" :
            cardType == CARD_SDHC ? "SDHC" :
            "UNKNOWN");
        if (cardType != CARD_NONE)
        {
            gdbgUint64("SD Card Size: %2.1f %s",SD.cardSize());
            gdbgUint64("Total space:  %2.1f %s",SD.totalBytes());
            gdbgUint64("Used space:   %2.1f %s",SD.usedBytes());
        }
    #else
        gdbgUint64("SPIFFS Total space: %2.1f %s",SPIFFS.totalBytes());
        gdbgUint64("SPIFFS Used space:  %2.1f %s",SPIFFS.usedBytes());
    #endif
}




//-----------------------------
// listDir()
//-----------------------------

bool removeDir(const char *dir, bool recursive=false, bool verbose=1, int level=0)
{
    if (!level || verbose)
        gdbg_sendf("removeDir(%s,%d)",dir,recursive);
    fileType root = fileSystem.open(dir);
    if (root && root.isDirectory())
    {
        fileType file = root.openNextFile();
        while (file)
        {
            char format[25];
            if (file.isDirectory())
            {
                if (recursive)
                    removeDir(file.name(),true,verbose,level+1);
            }
            else
            {
                if (verbose)
                    gdbg_sendf("deleting %s",file.name());
                if (!fileSystem.remove(file.name()))
                {
                    gdbg_sendf("error - removeDir could not remove %s",file.name());
                    return false;
                }
            }
            file = root.openNextFile();
        }
        root.close();
        if (!fileSystem.rmdir(dir))
        {
            gdbg_sendf("error - removeDir could not rmdir %s",dir);
            return false;
        }
    }
    else
    {
        gdbg_sendf("error - removeDir could not open(dir) %s",dir);
        return false;
    }
    return true;
}


void listDir(const char *dir, bool recursive/*=false*/,int level/*=0*/)
{
    if (!level)
        gdbg_sendf("\ndirectory listing of %s",dir);

    fileType root = fileSystem.open(dir);
    if (root && root.isDirectory())
    {
        fileType file = root.openNextFile();
        while (file)
        {
            char format[25];
            if (file.isDirectory())
            {
                sprintf(format,"%%%ds%%s/",level * 4);
                gdbg_sendf(format,"",file.leafName());
                if (recursive)
                    listDir(file.name(),true,level+1);
            }
            else
            {
                sprintf(format,"%%%ds%%-30s %%d ",level *4);
                gdbg_sendf(format,"",file.leafName(),file.size());
            }
            file = root.openNextFile();
        }
    }
    else
        gdbg_sendf("error - could not opendir(%s)",dir);

    if (root)
        root.close();
    gdbg_send("");

    #ifdef WITH_UI
        // remove old one first
        if (!level)
        {
            if (sp_uiDirectory)
                delete sp_uiDirectory;
            sp_uiDirectory = new uiDirectory(
                the_app->getContentArea(),
                fileSystem);
            sp_uiDirectory->navigate(dir);
        }
    #endif

}   // listDir()



//-------------------------------------------
// createTestData
//-------------------------------------------
// SPIFFS filenames limited to xx chars

#define test_data        "/test"
#define test_junk1        "/test/junk1"
#define test_junk2        "/test/junk2"
#define test_junk1_junk2  "/test/junk1/junk2"
#define test_file1        "file1"
#define test_file2        "file2"
#define test_file3        "file3"
#define PS                "/"

const char *makeStaticFilename(const char *dir, const char *leaf)
{
    #define NUM_STATIC_FILENAMES  10
    static int fileptr = 0;
    static char filename[NUM_STATIC_FILENAMES][255];
    char *rslt = filename[fileptr++];
    if (fileptr == NUM_STATIC_FILENAMES) fileptr = 0;
    sprintf(rslt,"%s%s%s",dir,PS,leaf);
    return rslt;
}

const char *makedirs[] = {
    test_data,
    test_junk1,
    test_junk2,
    test_junk1_junk2,
};

const char *makefiles[] = {
    test_file1,
    test_file2,
    test_file3,
};

#define NUM_MAKE_DIRS    (sizeof(makedirs)/sizeof(const char *))
#define NUM_MAKE_FILES   (sizeof(makefiles)/sizeof(const char *))

bool fillFile(fileType &file)
{
    for (int i=0; i<FILL_BYTES; i++)
    {
        char c = (i % 26) + 'A';
        if (i % 80 == 0) c = '\n';
        int rslt = file.write(c);
        if (rslt != 1)
        {
            gdbg_sendf("rslt(%d) Could not write byte(%d) '%c' to %s",
                rslt,
                i,
                c>32?c:' ',
                file.name());
            return false;
        }
    }
    return true;
}


bool createTestData()
{
    gdbg_send("\n---- initial listing");
    listDir(test_data,true);

    gdbg_send("\n---- createTestData()");
    for (int i=0; i<NUM_MAKE_DIRS; i++)
    {
        const char *dir_make = makedirs[i];
        if (!fileSystem.exists(dir_make))
        {
            gdbg_sendf("making dir %s",dir_make);
            if (!fileSystem.mkdir(dir_make))
            {
                gdbg_sendf("Could not mkdir(%s)",dir_make);
                return false;
            }
        }
    }

    for (int i=0; i<NUM_MAKE_DIRS; i++)
    {
        const char *dir_make = makedirs[i];
        for (int j=0; j<NUM_MAKE_FILES; j++)
        {
            const char *file_make = makefiles[j];
            const char *make_file = makeStaticFilename(dir_make,file_make);
            if (!fileSystem.exists(make_file))
            {
                gdbg_sendf("making file %s",make_file);
                fileType file = fileSystem.open(make_file,"w");
                if (!file)
                {
                    gdbg_sendf("Could not open(%s) for writing",make_file);
                }
                else
                {
                    bool ok = 1;
                    for (int k=0; ok && k<=j; k++)
                        ok &= fillFile(file);
                    file.close();
                    if (!ok)
                        return false;
                }
            }
        }
    }

    gdbg_send("\n---- data created");
    listDir(test_data,true);
    return true;

}   // createTestData()


//-------------------------------------------
// doTests()
//-------------------------------------------

bool testSanctity(const char *path, const char *open_mode, const char *parent_path)
{
    gdbg_sendf("\ntestSanctity of %s vs %s",path,parent_path);
    fileType file = fileSystem.open(path,open_mode);
    if (file)
    {
        gdbg_sendf("    path(%s) opened(%s)  isDirectory(%d)",file.name(),open_mode,file.isDirectory());

        if (!strcmp(open_mode,"w") && !file.isDirectory())
            fillFile(file);

        // try removing self while file is open

        if (file.isDirectory())
        {
            gdbg_send("    rmdir self while opened");
            if (fileSystem.rmdir(path))
                gdbg_send("        ERROR: rmdir self was allowed!");
        }
        else
        {
            gdbg_send("    remove self while opened");
            if (fileSystem.remove(path))
                gdbg_send("        ERROR: remove self was allowed!");
        }

        // try renaming self while opened

        const char *other_name = makeStaticFilename(parent_path,"blah");
        gdbg_sendf("    rename self to %s while opened",other_name);
        if (fileSystem.rename(path,other_name))
            gdbg_sendf("        ERROR: rename self to %s was allowed!",other_name);

        // try removing the parent path while opened

        gdbg_sendf("    rmdir %s while child is open",parent_path);
        if (fileSystem.rmdir(parent_path))
            gdbg_sendf("        ERROR: rmdir %s was allowed!",parent_path);


        // try renaming the parent path while opened

        gdbg_sendf("    rename %s to %s while child is open",parent_path,other_name);
        if (fileSystem.rename(parent_path,other_name))
            gdbg_sendf("        ERROR: rename(%s,%s) was allowed!",parent_path,other_name);

        // try renaming something else to this name while opened

        const char *exist_file = makeStaticFilename(test_data,test_file1);
        gdbg_sendf("    rename %s to %s while its open",exist_file,path);
        if (fileSystem.rename(exist_file,path))
            gdbg_sendf("        ERROR: rename(%s,%s) was allowed!",exist_file,path);

        // try renaming something else to the parent path while opened

        gdbg_sendf("    rename %s to %s while its open",exist_file,parent_path);
        if (fileSystem.rename(exist_file,path))
            gdbg_sendf("        ERROR: rename(%s,%s) was allowed!",exist_file,parent_path);

        file.close();
    }
    else
    {
        gdbg_sendf("    error - could not open path(%s,%s)",path,open_mode);
        return false;
    }
    return true;
}



void doTests()
{
    gdbg_send("\n---- doing tests");

    testSanctity(test_junk1_junk2,"w",test_junk1);
    testSanctity(test_junk1_junk2,"r",test_junk1);
    testSanctity(makeStaticFilename(test_junk1_junk2,test_file1),"w",test_junk1);
    testSanctity(makeStaticFilename(test_junk1_junk2,test_file1),"r",test_junk1);

    gdbg_send("---- tests completed\n");
    listDir(test_data,true);

}





//-------------------------------------------
// loop()
//-------------------------------------------

void loop()
{
    #ifdef TEST_SDCARD
        static bool testing = 0;
    #endif

    if (Serial.available())
    {
        int c = Serial.read();
        gdbg_sendf("\ncommand=%c",c>32?c:' ');

        if (c == 'l')
        {
            listDir(test_data,true);
        }
        else if (c == 'L')
        {
            listDir("/",true);
            showFSStats();
        }
        else if (c == 't')
        {
            doTests();
        }
        #ifdef TEST_SDCARD
            // only test polling and removal on SDCard
            else if (c == 'T')
            {
                testing = !testing;
                gdbg_sendf("testing(%d)",testing);
            }
        #endif
        else if (c == 'c')
        {
            createTestData();
        }
        else if (c == 'z')
        {
            removeDir(test_data,true,true);
        }
    }

    #ifdef TEST_SDCARD
        if (testing)
        {
            uint32_t now = millis();
            static bool do_test = 1;
            static int loop_counter = 0;
            static uint32_t test_timer = 0;
            if (now > test_timer + 10000)
            {
                test_timer = now;
                do_test = !do_test;
                if (do_test)
                    doLoopTest();
                else
                    gdbg_sendf("waiting %d",loop_counter++);
            }
        }
    #endif

}   // loop()


#ifdef TEST_SDCARD
    void doLoopTest()
    {
        const char *test_filename = makeStaticFilename(test_junk2,test_file2);
        fileType file = fileSystem.open(test_filename);
        if (!file)
            gdbg_sendf("Could not open %s in loop()",test_filename);
        else
        {
            gdbg_sendf("%s pos(%d) size(%d) avail(%d)",
                test_filename,
                file.position(),
                file.size(),
                file.available());

            int c = file.read();
            if (c == GRBL_FILE_ERROR)
            {
                gdbg_sendf("file started with GRBL_FILE_ERROR pos(%d) size(%d) avail(%d)",
                    file.position(),
                    file.size(),
                    file.available());

                c = file.read();
                gdbg_sendf("addl_read(%d) pos(%d) size(%d) avail(%d)",
                    c,
                    file.position(),
                    file.size(),
                    file.available());
                c = 0;
            }

            int bytes_read = 0;
            int retry = 0;
            while (c >= 0)
            {
                c = file.read();
                bytes_read++;

                if (0 && c == GRBL_FILE_ERROR && retry++ < 20)
                {
                    gdbg_sendf("retry(%d) pos(%d) size(%d) avail(%d)",
                        retry,
                        file.position(),
                        file.size(),
                        file.available());
                    // file,seek(0,SeekSet);
                    vTaskDelay(1000);
                    c = 0;
                }

                vTaskDelay(1);     // to give time to hit file while writing
            }
            if (c != GRBL_FILE_EOF)
                gdbg_sendf("file_test(%d) bytes(%d) pos(%d) size(%d) avail(%d) %s",
                    c,
                    bytes_read,
                    file.position(),
                    file.size(),
                    file.available(),
                    test_filename);
            file.close();
        }
    }   // doLoopTest()
#endif  // TEST_SDCARD