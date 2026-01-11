package task

import (
	"context"
)

// Task represents a work unit that downloads content from the server or other location
// Implementations can either be written in Go as internal functions or can call external CLI tools as required
type Task interface {
	// Execute performs the download. The provided context allows cancellation of the operation
	// and other sub operations if required
	Execute(ctx context.Context)
}
