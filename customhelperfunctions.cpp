#include "customhelperfunctions.h"
#include <stdio.h>
#include <string.h>

int get_usb_devices(std::vector<std::string> & devices){
    const char* systemcommand = "ls /dev | grep \"ttyUSB\"";

    //system(systemcommand.data());

    FILE *fp;
    char path[1035];

    int lines = 0;

    /* Open the command for reading. */
    fp = popen(systemcommand, "r");
    if (fp == NULL) {
      devices.push_back("Failed to run command\n" );
      lines = 0;
    }

    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path), fp) != NULL) {
      path[strcspn(path, "\n")] = 0;
      devices.push_back(path);
      //devices.end()->erase(devices.end.size()-1);//mystring.erase(mystring.size() - 1);
      lines ++;
    }

    /* close */
    pclose(fp);

    return lines;
}

customHelperFunctions::customHelperFunctions()
{

}
