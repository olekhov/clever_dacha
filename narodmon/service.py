#!env python
# -*- coding: utf-8  -*-

import psycopg2
import socket

import sys, time
from datetime import datetime, timedelta

import rpi_config


class NarodMonDaemon:
    """
    Класс для отправки данных в народный мониторинг
    """


    def __init__(self, connstring, apikey):
        """ Инициализация: connectionstring,  apikey """
        self.connstring = connstring
        self.apikey = apikey

    def run(self):
        """ Основной цикл """

        print "Засылаем данные.. Нажмите Ctrl-C для остановки"

        while 1:
            try:
		conn = psycopg2.connect(self.connstring)
                cur=conn.cursor()
                cur.execute("SET TIME ZONE 'Europe/Moscow';")
		w=datetime.now()-timedelta(minutes=10)
                window=w.strftime("%s")
                cur.execute('SELECT "T", "RH", "ts" FROM temperature WHERE (ts>%s) ORDER BY "ts" DESC;', (w,) )
                x=cur.fetchone()
                i=0
                if x :
                    print ("%3.1f %3.1f%% %s" % (x[0], x[1], x[2]))
                    try:
                        # создание сокета
                        sock = socket.socket()

                        # подключаемся к сокету
                        sock.connect(('narodmon.ru', 8283))

                        s=("#{0}\n#{1}#{2}\n#{3}#{4}\n##".format(rpi_config.pi_mac, 'T', x[0], 'RH', x[1]))
                        sock.send(s)
                        
                        # читаем ответ
                        data = sock.recv(1024)                 
                        print data
                    except socket.error:
                        print "socket error"
                    finally:
                        sock.close
                else:
                    print "None!"

                cur.close()
                conn.close()

                time.sleep(600);
            except KeyboardInterrupt:
                print "Bye"
                break


#            except Exception as e:
#                print "Oops:", sys.exc_info()[0]
#                print "e:",e
#                continue





if 1 :
#try:
    connstring = "dbname='%s' user='%s' host='%s' password='%s'" % (rpi_config.pg_db, rpi_config.pg_user, rpi_config.pg_host, rpi_config.pg_pass)
    print "Launching.."

    daemon = NarodMonDaemon(connstring,rpi_config.api_key)

    daemon.run()


#except Exception as e:
#    print "OopsMain:", sys.exc_info()[0]
#    print "E:", e

print "Done."





# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
