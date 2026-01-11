package download

import (
	"context"
	"log/slog"
	"net/url"
	"sync"

	"github.com/ananthvk/godown/internal/download/reporter"
	"github.com/ananthvk/godown/internal/download/storage"
	"github.com/ananthvk/godown/internal/download/task"
)

// Downloader manages downloading files concurrently from URLs.
// It uses a WriterFactory to allow the tasks to create writers for saving files,
// and a WaitGroup to wait until all downloads are complete. The ignoreInvalidURL flag controls
// whether invalid URLs are skipped or does not allow any download
type Downloader struct {
	writerFactory    storage.WriterFactory
	wg               sync.WaitGroup
	ignoreInvalidURL bool
	progressBar      reporter.ProgressBarFactory
}

// NewDownloader creates and returns a pointer to a Downloader object.
// It sets the WriterFactory to the default FSWriterFactory with the given basePath
// The ignoreInvalidURL flag determines whether invalid URLs are skipped or treated as errors
func NewDownloader(basePath string, ignoreInvalidURL bool, progressBarFactory reporter.ProgressBarFactory) *Downloader {
	downloader := Downloader{}
	downloader.writerFactory = &storage.FSWriterFactory{BasePath: basePath}
	downloader.ignoreInvalidURL = ignoreInvalidURL
	downloader.progressBar = progressBarFactory
	return &downloader
}

// Download downloads the file at urlString, it creates the appropriate DownloadTask
// depending upon the scheme in the url. Currently only HTTP(S) URLs are supported.
// If ignoreInvalidURL is true and the url lacks a scheme, "http://" is prepended.
// The task is executed in a separate goroutine and increments the value of the WaitGroup.
// Clients must call Wait() to ensure all downloads complete
func (d *Downloader) Download(ctx context.Context, urlString string) {
	if !d.ignoreInvalidURL && !IsUrl(urlString) {
		slog.Error("invalid url", "url", urlString)
		return
	}
	url, _ := url.Parse(urlString)
	var t task.Task

	switch url.Scheme {
	case "http", "https":
		t = &task.HTTPDownloadTask{Url: urlString, WriterFactory: d.writerFactory, ProgressBarFactory: d.progressBar}
	default:
		if d.ignoreInvalidURL && url.Scheme == "" {
			// If the url is invalid because it lacks a URL scheme, try adding a default http:// scheme
			t = &task.HTTPDownloadTask{Url: "http://" + urlString, WriterFactory: d.writerFactory, ProgressBarFactory: d.progressBar}
		} else {
			slog.Error("unsupported url scheme", "scheme", url.Scheme)
			return
		}
	}

	d.wg.Add(1)
	go func() {
		t.Execute(ctx)
		defer d.wg.Done()
	}()
}

// Wait blocks until all downloads started by Download() are complete
func (d *Downloader) Wait() {
	d.wg.Wait()
}
