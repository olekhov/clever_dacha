-- Table: temperature1

DROP TABLE IF EXISTS temperature;
DROP TABLE IF EXISTS sensors;

CREATE TABLE sensors
(
        id serial NOT NULL PRIMARY KEY,
        name varchar(200)
);
ALTER TABLE sensors
        OWNER TO cd_user;

CREATE TABLE temperature
(
        id serial NOT NULL PRIMARY KEY,
        "T" real,
        "RH" real,
        ts timestamp without time zone NOT NULL DEFAULT now(),
        sensor_id integer references sensors(id)
)
WITH (
          OIDS=FALSE
);

ALTER TABLE temperature
  OWNER TO cd_user;

INSERT INTO sensors(name) values ('Снаружи');
INSERT INTO sensors(name) values ('Внутри');
