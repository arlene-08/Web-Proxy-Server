# Web-Proxy-Server

Create a C-based client-server architecture using sockets. The proxy server should be able to accept and service single client’s http requests. The server should be on run on cell01 machine and clients should be able to run on any machine between cell02 and cell06. The proxy server should be able to cache at least five recently requested webpages if available. 

To compile:
pserver.c gcc pserver.c -o pserver ./pserver 80

client.c gcc client.c -o client ./client 80 url:"url"
