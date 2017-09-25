#ifndef GSFINPUTISTREAM_H
#define GSFINPUTISTREAM_H

#include "istream.h"
#include <gsf/gsf-fwd.h>
#include <gsf/gsf-infile.h>

/*
 * Extending Libgsf to support Istream.
 */

G_BEGIN_DECLS

typedef struct _GsfInputIStream GsfInputIStream;

#define GSF_INPUT_ISTREAM_TYPE        (gsf_input_istream_get_type ())
#define GSF_INPUT_ISTREAM(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), GSF_INPUT_ISTREAM_TYPE, GsfInputIStream))
#define GSF_IS_INPUT_ISTREAM(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), GSF_INPUT_ISTREAM_TYPE))

GType gsf_input_istream_get_type      (void) G_GNUC_CONST;

GsfInput *gsf_input_istream_new(mip::file::IStream *stream);
mip::file::IStream *gsf_input_istream_get_stream(GsfInput *input);

G_END_DECLS

#endif // GSFINPUTISTREAM_H
