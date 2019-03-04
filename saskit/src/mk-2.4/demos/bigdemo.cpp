/*
 * A demo that creates a file with a large number of rows
 * and shows how to use blocked ordered views to ensure efficient
 * fast access.
 *
 * usage: bigdemo -create NUMROWS FILENAME
 *        bigdemo -check ROWNUM FILENAME
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mk4.h>

static void
randfill(char *data, size_t len)
{
    for (size_t n = 0; n < len; ++n)
        *data++ = char(rand() * 26 + 95);
}

int
main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("usage: bigdemo -create|-check count filename\n");
        return 0;
    }
    
    int count = strtol(argv[2], NULL, 0);
    const char *filename = argv[3];
    
    // Access the file, declare the layout and open a view
    c4_Storage storage(filename, true);
    c4_View vRoot = storage.GetAs("items[_B[name:S,data:B]]");
    // Open the blocked version of this base view.
    c4_View vStuff = vRoot.Blocked();

    // Declare our property accessors.
    c4_StringProp rName("name");
    c4_BytesProp rData("data");
    
    if (strcmp("-create", argv[1]) == 0) {
        
        // Create some data to pad out the file.
        char name[34];
        char garbage[1024];
        randfill(garbage, sizeof(garbage));
        c4_Bytes data(garbage, sizeof(garbage));

        // Pre-size the view to speed up the creation
        vStuff.SetSize(count);
        for (int n = 0; n < count; ++n) {
            sprintf_s(name, sizeof(name), "item%d", n);
            c4_RowRef row = vStuff[n];
            rData(row) = data;
            rName(row) = name;
        }
        
        // Finally actually write this to a file.
        storage.Commit();

    } else {

        char name[34];
        sprintf_s(name, sizeof(name), "item%d", count);
        
        int row = vStuff.Find(rName[name]);
        printf("looking for '%s' returned row %d\n", name, row);
        if (row == -1)
            printf("error\n");
        else
            printf("found '%s'\n", (const char *)rName(vStuff[row]));

    }
    return 0;
}
