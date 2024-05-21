cmake_minimum_required(VERSION 3.28)
project(e_payment_iot)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(msgq)
include_directories(produce_msq_upstash)
include_directories(receive_msq_ustash)
include_directories(WiFiSMS)

add_executable(e_payment_iot
        msgq/msgq.ino
        produce_msq_upstash/produce_msq_upstash.ino
        produce_msq_upstash/utilities.h
        receive_msq_ustash/receive_msq_ustash.ino
        WiFiSMS/WiFiSMS.ino)
