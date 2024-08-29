
# psync

This is not an actual implementation of rendezvous protocol, but rather an attempt to create something similar and at the same time simple. This project has only one purpose, to let the peers (that are behind nat) discover each other.




## Build

Clone the project

```bash
  git clone https://link-to-project
```

Go to the project directory

```bash
  cd my-project
```

Compile it

```bash
  make
```

Start the server

```bash
  ./server.out -4v -p 1024
```


## How it works?

When you start the server, it begins listening on the specified UDP port. Once a connection is received, the server waits for an 8-byte message from the peer. This message should include a 4-byte self_id and a 4-byte peer_id, which allows peers to discover each other.

The structure looks like this: 
```c
struct {
  uint32_t self_id;
  uint32_t peer_id;
};
```

When another peer connects, the server generates two random ports and sends them to the peers. This means that Peer1 will use Port1 as its source port and Port2 as its destination port, while Peer2 will use Port2 as its source port and Port1 as its destination port. Both peers will also receive each other’s public IP addresses, as the sockaddr_in struct will be transferred along with this information.

The structure looks like this:

```c
struct {               
  uint16_t src_port;
  uint16_t dst_port                                                                           
  struct sockaddr_in dst_addr;
};
```

After disconnection, both peers will need to send something (let't say 1 byte segment) to each other in order to punch the hole in the nat table. 

I will use this configuration for demonstration purposes:

- Peer1: ip = 1.1.1.1; src_port = 4949; dst_port = 4948
- Peer2: ip = 2.2.2.2; src_port = 4948; dst_port = 4949

With netcat syntax it should look something like this:

```bash
# for peer1
printf "\xDE\xAD\xBE\xEF" | nc -u -p 4949 2.2.2.2 4948
```
```bash
# for peer2
printf "\xCA\xFE\xBA\xBE" | nc -u -p 4948 1.1.1.1 4949
```


## License

[MIT](https://choosealicense.com/licenses/mit/)
