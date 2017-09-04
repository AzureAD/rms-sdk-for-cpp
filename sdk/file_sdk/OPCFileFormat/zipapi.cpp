#include <algorithm>
#include <vector>
#include <gsf/gsf.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-utils.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-infile-zip.h>
#include <glib/gmessages.h>
#include <stdio.h>
#include "zipapi.h"
#undef signals
using std::vector;
using std::string;

//docProps/custom.xml
    std::string GetEntryImpl (GsfInput *input, std::string EntryPath)
    {
        GsfInput* entry = GSF_INPUT(gsf_infile_child_by_aname(GSF_INFILE(input),(const char**) g_strsplit(EntryPath.c_str(), "/",-1)));
        int size = gsf_input_size(entry);
        guint8* data = new guint8[size];

          gsf_input_read(entry,size, data);
         return std::string((const char*) data, size);
    }

    std::string ZipApi::GetEntry(std::string filePath, std::string EntryPath)
    {
       gsf_init ();

       GsfInput  *input;
       GsfInfile *infile;
       GError    *err = NULL;

       input = gsf_input_stdio_new (filePath.c_str(), &err);
       if (input == NULL) {

           g_return_val_if_fail (err != NULL, "");

           g_warning ("'%s' error: %s", filePath, err->message);
           g_error_free (err);
           err = NULL;
           return "";
       }

        infile = gsf_infile_zip_new (input, &err);
        if (infile == NULL) {

            g_return_val_if_fail (err != NULL,"");

            g_warning ("'%s' Not a Zip file: %s", filePath, err->message);
            g_error_free (err);
            err = NULL;
            return "";
        }
        std::string data = GetEntryImpl (GSF_INPUT (infile), EntryPath);
        g_object_unref (G_OBJECT (infile));
        g_object_unref (G_OBJECT (input));
        gsf_shutdown();
        return data;
    }

    gboolean test_write_once (GsfOutput *output)
    {
        char const *str = "Ameer Assi";

        return gsf_output_write (output, strlen (str), (const guint8*) str);
    }

    int ZipApi::SetEntry(std::string filePath, std::string EntryPath, std::string data)
    {
        gsf_init();
        GsfOutput  *output;
        GsfOutfile *outfile;
        GError   *err = NULL;
        FILE* file = fopen(filePath.c_str(), "r+");
        output = gsf_output_stdio_new_FILE(filePath.c_str(), file, false);
        if (output == NULL) {
             g_return_val_if_fail (err != NULL, 1);

             g_warning ("'%s' error: %s", filePath.c_str(), err->message);
             g_error_free (err);
             return 1;
         }
         outfile = gsf_outfile_zip_new (output, &err);
         if (outfile == NULL) {
             g_return_val_if_fail (err != NULL, 1);

             g_warning ("'%s' error: %s",
                    "gsf_outfile_zip_new", err->message);
             g_error_free (err);
             return 1;
         }
         g_object_unref (G_OBJECT (output));
         //GsfOutput *child1 = gsf_outfile_new_child  (outfile, "child", true);
         GsfOutput  *child = gsf_outfile_new_child  (outfile, "child", false);

         if (!test_write_once (child))
             return 1;
         if (!gsf_output_close ((GsfOutput *) child))
             return 1;
         if (!gsf_output_close ((GsfOutput *) outfile))
             return 1;
         g_object_unref (G_OBJECT (outfile));
         //g_object_unref (child1);
         g_object_unref (child);
         gsf_shutdown();
         //fclose(file);

         return 0;
    }
