#!/usr/bin/env python3
# -*- coding: utf-8 -*-
""" Rudimentary UDP log receiver """

import sys
import socket
import logging
import argparse

if __name__  == '__main__':

    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-p', '--port', required=False, type=int, default=6309,
                        help='UDP port to listen to for logging')
    pargs = parser.parse_args()

    # Configure logging
    logger = logging.getLogger(__name__)
    logger.setLevel(logging.DEBUG)
    h = logging.StreamHandler(sys.stdout)
    h.setFormatter(logging.Formatter("%(asctime)-15s %(levelname)s %(message)s"))
    logger.addHandler(h)

    UDP_IP  = '0.0.0.0'
    UDP_PORT = pargs.port

    logger.info(f'Listening to {UDP_IP}:{UDP_PORT}')

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

    while True:
        data, addr = sock.recvfrom(1024)

        try:
            split_msg = data.decode('utf-8').split(' ')
            level, msg = split_msg[0], ' '.join(split_msg[1:])

            msg = '[%s:%s] %s' % (addr[0], addr[1], msg)

            if 'DEBUG' in level:
                logger.debug(msg)
            if 'INFO' in level:
                logger.info(msg)
            if 'WARNING' in level:
                logger.warning(msg)
            if 'ERROR' in level:
                logger.error(msg)
            if 'CRITICAL' in level:
                logger.critical(msg)
        except:
            try:
                logger.info(data.decode('utf-8'))
            except:
                logger.info(repr(data))
