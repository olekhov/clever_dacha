#!/usr/bin/env python
# -*- coding: utf-8  -*-

import rpi_config

import psycopg2, serial

import sys, time, datetime


class CleverDachaDaemon:
    """
    Класс для считывания температуры, влажности и пр.
    Загружает считанные значения в БД
    """


    def __init__(self, connstring, tty):
        """ Инициализация: connectionstring, serial.Serial """
        self.connstring = connstring
        self.tty = tty

    def run(self):
        """ Основной цикл опроса датчиков """
        f=open("/var/db/temperature/output.txt","a")

        print "Обслуживаем... Нажмите Ctrl-C для остановки"

        while 1:
            try:
                time.sleep(10);
#                self.tty.open()
                self.tty.write(b'read')
                r=self.tty.readline().split()         
                print "Read: ",r
                f.write("{0} {1}\n".format(datetime.datetime.now(), r))
                f.flush()
#                self.tty.close()

                conn = psycopg2.connect(self.connstring)

                cur=conn.cursor()
                if len(r)>=2 :
                    cur.execute('INSERT INTO temperature("T","RH", sensor_id) VALUES(%s,%s,1)', (r[0], r[1],) )
                if len(r)>=4 :
                    cur.execute('INSERT INTO temperature("T","RH", sensor_id) VALUES(%s,%s,2)', (r[2], r[3],) )

                conn.commit()
                cur.close()
                conn.close()

            except KeyboardInterrupt:
                print "Bye"
                break

            except Exception as e:
                print "Oops:", sys.exc_info()[0]
                print "e:",e
                continue




#if 1 :
try:
    connstring = "dbname='%s' user='%s' host='%s' password='%s'" % (rpi_config.pg_db, rpi_config.pg_user, rpi_config.pg_host, rpi_config.pg_pass)
    tty = serial.Serial("/dev/ttyACM0")
    print "Launching.."
    daemon = CleverDachaDaemon(connstring,tty)
    daemon.run()

except Exception as e:
    print "OopsMain:", sys.exc_info()[0]
    print "E:", e

print "Done."





# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
