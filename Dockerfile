FROM ubuntu:20.10

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y build-essential