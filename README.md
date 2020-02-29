# network-benchmark
A simple server-client application running in Linux that sends a file via network using two possible flows on server side as below.

  -  Read file content from storage to a buffer in user space using file read syscall and send the file via UDP using socket sendmsg. The message must contain some sort of short header.
  -  Use sendfile syscall that transfers file content without copying the data to user space buffers.
