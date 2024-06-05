FROM alpine:latest as builder
RUN apk add alpine-sdk
COPY eptdump/* ./
RUN make

FROM alpine:latest
RUN apk --no-cache add u-boot-tools parted
COPY --from=builder eptdump /usr/bin/
COPY setpart init aml_env /
COPY inittab /etc/
RUN chmod +x /init

