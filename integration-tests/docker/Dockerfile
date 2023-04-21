FROM docker.io/library/ubuntu:22.10

RUN apt update -y
RUN apt install -y bind9
RUN mkdir -p /run/named
RUN chown -R bind.bind /run/named

CMD ["named", "-g", "-c", "/etc/bind/named.conf", "-u", "bind"]
