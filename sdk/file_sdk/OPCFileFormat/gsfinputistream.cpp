#include "gsfinputistream.h"
#include <gsf/gsf-infile-impl.h>
#include <gsf/gsf-impl-utils.h>

static GObjectClass *parent_class;

struct _GsfInputIStream {
    GsfInput input;
    IStream *stream;
    guint8 *buf;
    gsize buf_size;
};

typedef struct {
    GsfInputClass input_class;
} GsfInputIStreamClass;

GsfInput *gsf_input_istream_new(IStream *stream)
{
    GsfInputIStream *gsfStream;

    gsfStream = (GsfInputIStream*) g_object_new(GSF_INPUT_ISTREAM_TYPE, NULL);
    gsfStream->stream = stream;

    gsf_input_set_size(GSF_INPUT(gsfStream), gsfStream->stream->Size());
    gsf_input_set_name(GSF_INPUT(gsfStream), "ameer");

    return GSF_INPUT(gsfStream);
}

IStream *gsf_input_istream_get_stream(GsfInput *input)
{
    GsfInputIStream *gsfStream = GSF_INPUT_ISTREAM(input);

    return gsfStream->stream;
}

static void gsf_input_istream_finalize(GObject *obj)
{
    GsfInputIStream *gsfStream = GSF_INPUT_ISTREAM(obj);

    if(gsfStream->buf)
        g_free(gsfStream->buf);

    parent_class->finalize(obj);
}

static GsfInput *gsf_input_istream_dup(GsfInput *src_input, GError **err)
{
    GsfInputIStream *gsfStream = GSF_INPUT_ISTREAM(src_input);

    return gsf_input_istream_new(gsfStream->stream);
}

static const guint8 *gsf_input_istream_read(GsfInput *input, size_t num_bytes, guint8 *buffer)
{
    GsfInputIStream *gsfStream = GSF_INPUT_ISTREAM(input);

    if(buffer == NULL) {
        if(gsfStream->buf_size < num_bytes) {
            gsfStream->buf_size = num_bytes;
            if(gsfStream->buf)
                g_free(gsfStream->buf);
            gsfStream->buf = g_new0(guint8, num_bytes);
        }
        buffer = gsfStream->buf;
    }

    gsfStream->stream->Read(buffer, num_bytes);

    return buffer;
}

static gboolean gsf_input_istream_seek(GsfInput *input, gsf_off_t offset, GSeekType whence)
{
    GsfInputIStream *gsfStream = GSF_INPUT_ISTREAM(input);

    gsfStream->stream->Seek(offset);
    return TRUE;
}

static void gsf_input_istream_init(GObject *obj)
{
    GsfInputIStream *gsfStream = GSF_INPUT_ISTREAM(obj);

    gsfStream->stream = NULL;
    gsfStream->buf = NULL;
    gsfStream->buf_size = 0;
}

static void gsf_input_istream_class_init(GObjectClass *gobject_class)
{
    GsfInputClass *input_class = GSF_INPUT_CLASS(gobject_class);

    gobject_class->finalize = gsf_input_istream_finalize;
    input_class->Dup  = gsf_input_istream_dup;
    input_class->Read = gsf_input_istream_read;
    input_class->Seek = gsf_input_istream_seek;

    parent_class = (GObjectClass*) g_type_class_peek_parent(gobject_class);
}

GSF_CLASS(GsfInputIStream,
    gsf_input_istream,
    gsf_input_istream_class_init,
    gsf_input_istream_init,
    GSF_INPUT_TYPE)
