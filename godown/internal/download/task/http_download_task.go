package task

import (
	"context"
	"io"
	"log/slog"
	"mime"
	"net/http"
	"path"
	"path/filepath"

	"github.com/ananthvk/godown/internal/download/reporter"
	"github.com/ananthvk/godown/internal/download/storage"
)

// This is the default file name of the download when no file is detected from either the URL
// or from the header
const defaultFileName = "download"

// HTTPDownloadTask Implements Task and represents a HTTP(S) download
// Url is the resource to be fetched; WriterFactory creates WriterCloser streams
// to be used by the task to save the response to some location
type HTTPDownloadTask struct {
	Url                string
	WriterFactory      storage.WriterFactory
	ProgressBarFactory reporter.ProgressBarFactory
}

// Execute performs a HTTP GET request for the task's url and saves the response to the location.
// The URL is assumed to be valid.
// The passed context is used for cancelling the task if required.
// If the server returns with a status code < 200 or >= 300, the download is aborted.
// WriterFactory is used to create a WriteCloser stream to save the response to, and the filename is determined from the
// response header or the URL.
func (h *HTTPDownloadTask) Execute(ctx context.Context) {
	slog.Info("starting download", slog.String("url", h.Url))
	req, err := http.NewRequestWithContext(ctx, http.MethodGet, h.Url, nil)
	if err != nil {
		slog.Error("creating request", slog.String("url", h.Url), "err", err)
	}
	// TODO: Set a timeout
	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		slog.Error("starting download", slog.String("url", h.Url), "err", err)
		return
	}
	defer resp.Body.Close()

	if resp.StatusCode >= 200 && resp.StatusCode <= 299 {
		slog.Info("http request sent", "status", resp.Status, "url", h.Url)
	} else {
		slog.Error("http request sent", "status", resp.Status, "url", h.Url)
		return
	}

	fileName := getFileName(resp)

	fileName, dest, err := h.WriterFactory.CreateStream(fileName)
	if err != nil {
		slog.Error("failed to create write stream", "url", h.Url, "filename", fileName, "err", err)
		return
	}

	total := resp.ContentLength
	if total <= 0 {
		total = 0
		slog.Info("content length header not found", "url", h.Url, "length", resp.ContentLength)
	} else {
		slog.Info("content length header found", "url", h.Url, "length", resp.ContentLength)
	}

	bar := h.ProgressBarFactory.CreateProgressBar(total, "Download "+fileName)

	r := bar.ProxyReader(resp.Body)
	if r == nil {
		slog.Error("failed to create progress bar proxy reader", "url", h.Url, "filename", fileName, "err", err)
		r = resp.Body
	}

	b, err := io.Copy(dest, r)
	if err != nil {
		slog.Error("failed to save response", "url", h.Url, "filename", fileName, "err", err)
		bar.Abort(true)
		return
	}

	if total == 0 {
		bar.SetTotal(-1, true)
	}

	slog.Info("finished download", "url", h.Url, "filename", fileName, "bytes", b)
}

// getFileName returns the filename from the response
// It first checks the Content-Disposition header;
// if it's missing or invalid, it attempts to infer the filename from the URL
func getFileName(resp *http.Response) string {
	fileName := getFileNameFromHeader(resp)
	if fileName == "" {
		fileName = getFileNameFromURL(resp)
		slog.Info("resolved filename from url", "filename", fileName)
	} else {
		slog.Info("resolved filename from http header", "filename", fileName)
	}
	return fileName
}

// getFileNameFromHeader returns the filename from the Content-Disposition header of the response.
// If the header is missing or invalid, or does not contain a "filename" key, an empty string is returned
func getFileNameFromHeader(resp *http.Response) string {
	contentDisposition := resp.Header.Get("Content-Disposition")
	if contentDisposition == "" {
		return ""
	}
	_, params, err := mime.ParseMediaType(contentDisposition)
	if err != nil {
		return ""
	}
	if fileName, ok := params["filename"]; ok {
		return fileName
	}
	return ""
}

// getFileNameFromURL returns the filename from the base (or last segment of the URL).
// If the last segment is empty or is a slash "/", defaultFileName is used as the filename.
// If the filename does not have an extension, it attempts to identify the file extension
// from the Content-Type header
func getFileNameFromURL(resp *http.Response) string {
	u := resp.Request.URL
	fileName := path.Base(u.Path)
	if fileName == "" || fileName == "." || fileName == "/" || fileName == ".." {
		fileName = defaultFileName
	}

	if filepath.Ext(fileName) == "" {
		fileName += getFileExt(resp)
	}

	return fileName
}

// getFileExt returns a file extension based on the Content-Type header of the response.
// If the header does not exist, or there is any error, an empty string is returned.
func getFileExt(resp *http.Response) string {
	contentType := resp.Header.Get("Content-Type")
	if contentType == "" {
		return ""
	}
	mediatype, _, err := mime.ParseMediaType(contentType)
	if err != nil {
		return ""
	}
	extensions, err := mime.ExtensionsByType(mediatype)
	if err == nil && len(extensions) > 0 {
		return extensions[0]
	}
	return ""
}
