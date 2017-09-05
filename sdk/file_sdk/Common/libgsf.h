#ifndef LIBGSF_H
#define LIBGSF_H

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

#endif // LIBGSF_H
