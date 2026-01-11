# godown

A CLI utility written in go that concurrently fetches multiple URLS

# Help
```
NAME:
   godown - A new cli application

USAGE:
   godown [global options] <url>

VERSION:
   0.0.1

DESCRIPTION:
   godown is a concurrent file downloader

GLOBAL OPTIONS:
   --output-dir string   directory to save files to (default: ".")
   --ignore-invalid-url  ignores invalid urls that are passed as input, if the input url is missing a scheme, automatically prepends http:// (default: false)
   --log                 Enables logging (default: false)
   --help, -h            show help
   --version, -v         print the version
```

## BUGS / TODO

- [ ] Progress bar gets stuck when the server closes unexpectedly
- [ ] Handle prealloacation of disk space
- [ ] Number of retries
- [ ] Handle timeout