package storage

import "io"

// WriterFactory creates writable streams for saving data to a storage backend
// Implementation should return a WriterCloser and may modify the filename (to avoid
// collisions if needed). Returns an error if the stream cannot be created
type WriterFactory interface {
	CreateStream(fileName string) (string, io.WriteCloser, error)
}
