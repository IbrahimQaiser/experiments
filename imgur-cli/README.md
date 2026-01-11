# imgur-cli
A cli tool to upload and delete images on [Imgur](https://imgur.com)

# Features
1. Anonymous upload of images to imgur.
2. Delete anonymous image uploads.

# Installation

Execute
```
go install github.com/ananthvk/imgur-cli@latest
```

# Getting the client id
This program requires a client id to work.
Get a client id from [https://api.imgur.com/oauth2/addclient](https://api.imgur.com/oauth2/addclient)

## If you are on windows
```
set IMGUR_CLIENT_ID=[CLIENT_ID]
```

## If you are on linux
```
$ export IMGUR_CLIENT_ID=[CLIENT_ID]
$ imgur-cli upload cat.png
```
Or
```
$ IMGUR_CLIENT_ID=[CLIENT_ID] imgur-cli upload cat.png
```
You can also place it in your shell profile, for example `~/.bashrc` or `~/.zshrc`

# Usage

## Anonymous upload of images to imgur
```
imgur-cli upload <path to image file>
```
### Example
```
$ IMGUR_CLIENT_ID=xxxxxxxx imgur-cli upload TBJLzvUqnh4i6vtAnhCs--1--lvv9j_2x.jpg
Success: uploaded image to imgur!
Image URL: https://i.imgur.com/Sl0hhG1.jpg
Delete hash: xxxxxxxxxxxx
Please keep the above delete hash safe as it is required to remove the image from imgur.
```
Note after uploading, a delete hash will be shown. Please copy it as it is required if you want to remove the image in the future.

## Delete uploaded images
```
imgur-cli delete <delete hash>
```