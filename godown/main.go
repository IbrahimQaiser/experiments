package main

import (
	"context"
	"io"
	"log/slog"
	"os"
	"os/signal"
	"syscall"

	"github.com/ananthvk/godown/internal/download"
	"github.com/ananthvk/godown/internal/download/reporter"
	"github.com/urfave/cli/v3"
	"github.com/vbauerster/mpb/v8"
)

func main() {
	cmd := (&cli.Command{
		Name:        "godown",
		Description: "godown is a concurrent file downloader",
		Version:     "0.0.1",
		ArgsUsage:   "<url>",
		Flags: []cli.Flag{
			&cli.StringFlag{
				Name:  "output-dir",
				Value: ".",
				Usage: "directory to save files to",
			},
			&cli.BoolFlag{
				Name:  "ignore-invalid-url",
				Value: false,
				Usage: "ignores invalid urls that are passed as input, if the input url is missing a scheme, automatically prepends http://",
			},
			&cli.BoolFlag{
				Name:  "log",
				Value: false,
				Usage: "Enables logging",
			},
		},
		Action: func(ctx context.Context, cmd *cli.Command) error {

			/*


				bar2 := p.New(int64(100),
					mpb.BarStyle().Lbound("#").Rbound("#").Tip(">").Padding("."),
					mpb.PrependDecorators(decor.Name("Prog bar")),
					mpb.AppendDecorators(decor.Percentage()),
				)

				max := 100 * time.Millisecond
				for i := 0; i < 100; i++ {
					time.Sleep(time.Duration(rand.Intn(10)+1) * max / 10)
					bar.Increment()
					time.Sleep(time.Duration(rand.Intn(10)+1) * max / 10)
					bar2.Increment()
				}
				p.Wait()
			*/
			if cmd.Args().Len() == 0 {
				return cli.Exit("no urls specified", 1)
			}

			if !cmd.Bool("log") {
				slog.SetDefault(slog.New(slog.NewTextHandler(io.Discard, nil)))
			}

			ctx, cancel := signal.NotifyContext(ctx, os.Interrupt, syscall.SIGTERM)
			defer cancel()

			progressBar := &reporter.MpbProgressBar{Progress: mpb.NewWithContext(ctx, mpb.WithWidth(64))}
			downloader := download.NewDownloader(cmd.String("output-dir"), cmd.Bool("ignore-invalid-url"), progressBar)

			for _, url := range cmd.Args().Slice() {
				downloader.Download(ctx, url)
			}

			slog.Info("waiting for all downloads to complete")
			downloader.Wait()
			progressBar.Progress.Wait()
			slog.Info("completed all downloads")
			return nil
		},
	})
	if err := cmd.Run(context.Background(), os.Args); err != nil {
		os.Exit(1)
	}
}
