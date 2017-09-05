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
#include "libgsf.h"
#undef signals
using std::vector;
using std::string;

static void clone_ (GsfInfile *in, GsfOutfile *out, int first)
{
  GsfInput *input = GSF_INPUT (in);
  GsfOutput *output = GSF_OUTPUT (out);

  if (gsf_input_size (input) > 0) {
    size_t len;
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
    }
  } else {
    int i, n = gsf_infile_num_children (in);
    for (i = 0 ; i < n; i++) {
      int level;
      gboolean is_dir;
      char const *name = gsf_infile_name_by_index (in, i);
      char *display_name = name
          ? g_filename_display_name (name)
          : NULL;

      input = gsf_infile_child_by_index (in, i);
      if (NULL == input) {
        g_print ("Error opening '%s, index = %d\n",
                 display_name ? display_name : "?", i);
        g_free (display_name);
        continue;
      }

      is_dir = gsf_infile_num_children (GSF_INFILE (input)) >= 0;

      g_object_get (G_OBJECT (input), "compression-level", &level, NULL);

      g_print ("%s: size=%ld, level=%d, %s\n",
               display_name ? display_name : "?",
               (long)gsf_input_size (input),
               level,
               is_dir ? "directory" : "file");
      g_free (display_name);

      output = gsf_outfile_new_child_full  (out, name, is_dir,
                                            "compression-level", level,
                                            NULL);
      clone_ (GSF_INFILE (input), GSF_OUTFILE (output), 0);
    }
  }
  if(first == 0)
  {
    gsf_output_close (GSF_OUTPUT (out));
    g_object_unref (G_OBJECT (out));
    g_object_unref (G_OBJECT (in));
  }
}

//docProps/custom.xml

std::string GetEntryImpl (GsfInfile *infile, std::string EntryPath)
{
  gchar** elems = g_strsplit(EntryPath.c_str(), "/", -1);
  std::unique_ptr<GsfInput, GsfInput_deleter> entry(gsf_infile_child_by_aname(infile, (const char**)elems));
  g_strfreev (elems);

  gsf_off_t size = gsf_input_size(entry.get());
  guint8* data = new guint8[size];

  gsf_input_read(entry.get(), size, data);
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
  std::string data = GetEntryImpl (infile, EntryPath);
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
  std::cout << "1" << std::endl;
  gsf_init();
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

  output = gsf_output_stdio_new("C:\\Users\\amassi\\Desktop\\aaaa1.docx", &err);
  if (output == NULL) {
    g_return_val_if_fail (err != NULL, 1);

    g_warning ("'%s' error: %s", filePath.c_str(), err->message);
    g_error_free (err);
    return 1;
  }
  std::cout << "2"<< std::endl;

  outfile = gsf_outfile_zip_new (output, &err);
  if (outfile == NULL) {
    g_return_val_if_fail (err != NULL, 1);

    g_warning ("'%s' error: %s",
               "gsf_outfile_zip_new", err->message);
    g_error_free (err);
    return 1;
  }
  std::cout << "3"<< std::endl;
  g_object_unref (G_OBJECT (output));
  clone_(GSF_INFILE(infile), GSF_OUTFILE(outfile), 1);
  GsfOutput *child1 = gsf_outfile_new_child  (outfile, "child", true);
  GsfOutput  *child = gsf_outfile_new_child  (GSF_OUTFILE(child1), "child", false);
  std::cout << "4"<< std::endl;

  // if (!test_write_once (child))
  //    return 1;
  std::cout << "5"<< std::endl;

  if (!gsf_output_close ((GsfOutput *) child))
    return 1;
  std::cout << "6"<< std::endl;

  if (!gsf_output_close ((GsfOutput *) outfile))
    return 1;
  std::cout << "7"<< std::endl;
  //fclose(f);
  g_object_unref (G_OBJECT (outfile));
  g_object_unref (child1);
  g_object_unref (child);
  gsf_shutdown();
  //fclose(file);

  return 0;
}
