package storage

import (
	"fmt"
	"io"
	"log/slog"
	"os"
	"path"
	"path/filepath"
	"strings"
	"sync"
)

// FSWriterFactory implements WriterFactory and creates streams to write to local file system.
// BasePath specifies the directory where files are created
// An internal Mutex is used to ensure that concurrent goroutines cannot create a stream to the same file.
type FSWriterFactory struct {
	BasePath string
	mu       sync.Mutex
}

// CreateStream creates a new WriterCloser stream that can be used to save the response body
// It returns the actual filename on the disk, this filename may not be same as the passed fileName
// incase of file conflicts.
// It also creates the necessary parent directories as required.
// This function also locks the Mutex so that concurrent goroutines do not get a stream to the same file.
// Incase of conflicts, the filename is modified as <filename> (number) . <extension>
func (f *FSWriterFactory) CreateStream(fileName string) (string, io.WriteCloser, error) {
	if len(f.BasePath) != 0 {
		err := os.MkdirAll(f.BasePath, 0755)
		if err != nil {
			return fileName, nil, err
		}

	}
	f.mu.Lock()
	defer f.mu.Unlock()

	filePath := path.Join(f.BasePath, fileName)
	fileNameNew := ""

	ctr := 1
	// While the file exists, check for new names

	for {
		exists, err := doesFileExist(filePath)
		if err != nil {
			return "", nil, err
		}
		if !exists {
			if fileNameNew != "" {
				fileName = fileNameNew
			}
			break
		}
		// TODO: Optimization: Store the filename along with the counter in a map so that the next time the free file name can be
		// found easily
		slog.Info("file exists", "path", filePath)
		ext := filepath.Ext(fileName)
		fileNameWithoutExt := strings.TrimSuffix(fileName, ext)
		fileNameNew = fmt.Sprintf("%s (%d)%s", fileNameWithoutExt, ctr, ext)
		filePath = path.Join(f.BasePath, fileNameNew)
		ctr++
	}

	file, err := os.Create(filePath)
	return fileName, file, err
}

// doesFileExist checks if a file exists at filePath location.
// It returns false if the file does not exist.
// It returns true if the file exists and os.Stat does not return an error.
// If os.Stat returns an error, the error is returned to the caller along with false
func doesFileExist(filePath string) (bool, error) {
	_, err := os.Stat(filePath)
	if os.IsNotExist(err) {
		return false, nil
	}
	return err == nil, err
}
