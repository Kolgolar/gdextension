#!/bin/bash

# Проверяем аргумент
if [ -z "$1" ]; then
  echo "Using: $0 <threads>"
  exit 1
fi

NUM_STREAMS=$1
BASE_PORT=5000

for ((i=0; i<NUM_STREAMS; i++)); do
  PORT=$((BASE_PORT + i))
  SSRC=$((RANDOM + i * 1000))
  OFFSET=$(( $(date +%s%N) + i * 1000000 ))

  gnome-terminal --title="GStreamer $PORT" -- bash -c \
  "gst-launch-1.0 -v videotestsrc is-live=true timestamp-offset=$OFFSET \
  ! video/x-raw,width=1920,height=1080,framerate=30/1 \
  ! videoconvert \
  ! video/x-raw,format=I420 \
  ! nvh264enc preset=hp \
  ! rtph264pay config-interval=1 pt=96 ssrc=$SSRC \
  ! udpsink host=127.0.0.1 port=$PORT; exec bash"
done

