package reporter

import (
	"io"

	"github.com/vbauerster/mpb/v8"
	"github.com/vbauerster/mpb/v8/decor"
)

type ProgressBarFactory interface {
	CreateProgressBar(total int64, name string) ProgressBar
}

type ProgressBar interface {
	ProxyReader(r io.Reader) io.ReadCloser
	SetTotal(total int64, complete bool)
	Abort(drop bool)
}

type MpbProgressBar struct {
	Progress *mpb.Progress
}

func (pb *MpbProgressBar) CreateProgressBar(total int64, name string) ProgressBar {
	return pb.Progress.New(total,
		mpb.BarStyle().Lbound("[").Rbound("]").Tip(">").Padding(".").Filler("="),
		mpb.PrependDecorators(
			decor.Name(name+": ", decor.WCSyncWidthR),
			decor.EwmaETA(decor.ET_STYLE_HHMMSS, 30, decor.WCSyncWidth),
		),
		mpb.AppendDecorators(decor.Percentage(), decor.Counters(decor.SizeB1024(0), " [% .1f / % .1f]")),
	)
}
