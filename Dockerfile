FROM alpine:latest as builder
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.tuna.tsinghua.edu.cn/g' /etc/apk/repositories && apk add alpine-sdk
COPY eptdump/* ./
RUN make

FROM alpine:latest
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.tuna.tsinghua.edu.cn/g' /etc/apk/repositories && \
    apk --no-cache add u-boot-tools parted btrfs-progs e2fsprogs-extra e2fsprogs
COPY --from=builder eptdump /usr/bin/
COPY setpart init aml_env /
COPY inittab /etc/
RUN chmod +x /init

