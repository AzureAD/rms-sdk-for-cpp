#include <algorithm>
#include <vector>
#include <gsf/gsf.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include "zipapi.h"

using std::vector;
using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::map;

struct GsfOutput_deleter {
    void operator () (GsfOutput* obj) const {
        if (!gsf_output_is_closed(obj)) {
            gsf_output_close(obj);
        }
        g_object_unref(G_OBJECT(obj));
    }
};

struct GsfOutfile_deleter {
    void operator () (GsfOutfile* obj) const {
        if (!gsf_output_is_closed(GSF_OUTPUT(obj))) {
            gsf_output_close(GSF_OUTPUT(obj));
        }
        g_object_unref(G_OBJECT(obj));
    }
};

struct GsfInput_deleter {
    void operator () (GsfInput* obj) const {
        g_object_unref(G_OBJECT(obj));
    }
};

struct GsfInfile_deleter {
    void operator () (GsfInfile* obj) const {
        g_object_unref(G_OBJECT(obj));
    }
};

struct GsfStrSplit_deleter {
    void operator () (gchar** obj) const {
        g_strfreev(obj);
    }
};

struct GError_deleter {
    void operator () (GError* obj) const {
        if(obj != nullptr)
            g_error_free(obj);
    }
};

ZipApi::ZipApi()
{
    gsf_init ();
}

string GetEntryImpl (GsfInfile *infile, string entryPath)
{
    unique_ptr<gchar*, GsfStrSplit_deleter> elems(g_strsplit(entryPath.c_str(), "/", -1));

    auto child = gsf_infile_child_by_aname(infile, (const char**)elems.get());
    if (child == nullptr)
        throw ZipEntryNotFoundException("Entry not found", entryPath);

    unique_ptr<GsfInput, GsfInput_deleter> entry(child);

    gsf_off_t size = gsf_input_size(entry.get());

    auto buf = gsf_input_read(entry.get(), size, nullptr);
    if (!buf)
        throw ZipException("Failed reading entry", entryPath);

    return string((const char*)buf, size);
}

string ZipApi::GetEntry(shared_ptr<IStream> stream, string entryPath)
{
    GError *err = nullptr;

    unique_ptr<GsfInput, GsfInput_deleter> input(gsf_input_istream_new (stream.get()));

    unique_ptr<GsfInfile, GsfInfile_deleter> infile(gsf_infile_zip_new (input.get(), &err));
    if (!infile) {
        throw ZipException("Not a valid zip file: %s", err->message);
        g_error_free (err);
    }

    string data = GetEntryImpl (infile.get(), entryPath);

    return data;
}

void clone(GsfInfile *in, GsfOutfile *out, const string& path, int first, map<string,string>& map)
{
    GsfInput *input = GSF_INPUT (in);
    GsfOutput *output = GSF_OUTPUT (out);
    if (gsf_input_size (input) > 0) {
        size_t len;
        std::map<string,string>::iterator it;
        auto fixedPath = path.substr(0, path.size() - 1);
        if((it = map.find(fixedPath)) != map.end())
        {
            if (!gsf_output_write (output, it->second.length(), (const guint8*) it->second.c_str())) {
                g_warning ("error writing ?");
            }
            map.erase(it);
        }
        else{
            while ((len = gsf_input_remaining (input)) > 0) {
                guint8 const *data;
                /* copy in odd sized chunks to exercise system */
                if (len > 314)
                    len = 314;
                if (NULL == (data = gsf_input_read (input, len, NULL))) {
                    g_warning ("error reading ?");
                    break;
                }
                if (!gsf_output_write (output, len, data)) {
                    g_warning ("error writing ?");
                    break;
                }
            }}
    } else {
        int i, n = gsf_infile_num_children (in);
        for (i = 0 ; i < n; i++) {
            int level;
            gboolean is_dir;
            char const *name = gsf_infile_name_by_index (in, i);

            input = gsf_infile_child_by_index (in, i);
            if (NULL == input) {
                // TODO: handle error.
                continue;
            }

            is_dir = gsf_infile_num_children (GSF_INFILE (input)) >= 0;

            g_object_get (G_OBJECT (input), "compression-level", &level, NULL);

            output = gsf_outfile_new_child_full  (out, name, is_dir,
                                                  "compression-level", level,
                                                  NULL);
            const string newPath = path + string(name) + "/";
            clone(GSF_INFILE (input), GSF_OUTFILE (output), newPath, 0, map);
        }
    }
    if(first == 0){
        gsf_output_close (GSF_OUTPUT (out));
        g_object_unref (G_OBJECT (out));
        g_object_unref (G_OBJECT (in));
    }
}

gboolean WriteData (GsfOutput *output, const char* data)
{
    return gsf_output_write (output, strlen (data), (const guint8*) data);
}

int ZipApi::SetEntry(const string& filePath, const string& outputFilePath, map<string,string>& entries)
{
    GsfOutput  *output;
    GsfOutfile *outfile;
    GError   *err = NULL;
    GsfInput  *input;
    GsfInfile *infile;

    input = gsf_input_stdio_new (filePath.c_str(), &err);
    if (input == NULL) {

        g_return_val_if_fail (err != NULL, 1);

        g_warning ("'%s' error: %s", filePath, err->message);
        g_error_free (err);
        err = NULL;
        return 1;
    }

    infile = gsf_infile_zip_new (input, &err);
    if (infile == NULL) {

        g_return_val_if_fail (err != NULL,1);

        g_warning ("'%s' Not a Zip file: %s", filePath, err->message);
        g_error_free (err);
        err = NULL;
        return 1;
    }

    output = gsf_output_stdio_new(outputFilePath.c_str(), &err);
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
    clone(GSF_INFILE(infile), GSF_OUTFILE(outfile), (gchar*)"", 1,entries);

    for (map<string,string>::iterator it= entries.begin(); it!= entries.end(); ++it)
    {
        gchar **elems = g_strsplit(it->first.c_str(), "/", -1);
        gchar **elems1 = elems;
        gchar *firstName = *elems;
        elems++;
        GsfOutput* prev_child = gsf_outfile_new_child  (GSF_OUTFILE(outfile), firstName, *elems != NULL);
        GsfOutput* cur_child = prev_child;

        while (*elems) {
            gchar * currentName = *elems;
            elems++;
            cur_child = gsf_outfile_new_child  (GSF_OUTFILE(prev_child), currentName, *elems != NULL);
            g_object_unref(prev_child);
            prev_child = cur_child;
        }

        g_strfreev (elems1);

        if (!WriteData (cur_child, it->second.c_str()))
            return 1;

        if (!gsf_output_close ((GsfOutput *) cur_child))
            return 1;
        g_object_unref (cur_child);
    }
    if (!gsf_output_close ((GsfOutput *) outfile))
        return 1;
    g_object_unref (G_OBJECT (outfile));

    return 0;
}
