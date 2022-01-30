import sqlite3
from sqlite3 import Error
import os
import json

def create_connection(db_file):
    """ create a database connection to the SQLite database
        specified by db_file
    :param db_file: database file
    :return: Connection object or None
    """
    conn = None
    try:
        conn = sqlite3.connect(db_file)
        return conn
    except Error as e:
        print(e)

    return conn


def create_table(conn, create_table_sql):
    """ create a table from the create_table_sql statement
    :param conn: Connection object
    :param create_table_sql: a CREATE TABLE statement
    :return:
    """
    try:
        c = conn.cursor()
        c.execute(create_table_sql)
    except Error as e:
        print(e)


def main():
    database = r"trails.db"

    trails_table = """ CREATE TABLE IF NOT EXISTS trail (
                                            id integer PRIMARY KEY,
                                            name text,
                                            link text,
                                            popularity numeric,
                                            latitude numeric,
                                            longitude numeric,
                                            length numeric,
                                            difficulty integer,
                                            rating numeric,
                                            area text,
                                            city_id integer,
                                            city_name text,
                                            state_id integer
                                    ); """

    # create a database connection
    conn = create_connection(database)

    # create tables
    if conn is not None:
        # create trails table
        create_table(conn, trails_table)
        return conn
    else:
        print("Error! cannot create the database connection.")
        return False

def add_state_trails(conn, state_file):
    print(state_file)

    sql = ''' INSERT OR IGNORE INTO trail(
                id, name, link, popularity, 
                latitude, longitude, length, 
                difficulty, rating, area, 
                city_id, city_name, state_id
            )
            VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    
    state_trails = open("trails/"+state_file, "r").read()
    data = json.loads(state_trails)
    
    state = 0
    for trail in data:
        if state == 0:
            state = [trail['state_name'], trail['state_id']]
            
        trail_data = (
            trail['ID'],
            trail['name'],
            trail['slug'],
            trail['popularity'],
            trail['_geoloc']['lat'],
            trail['_geoloc']['lng'],
            trail['length'],
            trail['difficulty_rating'],
            trail['avg_rating'],
            trail['area_name'],
            trail['city_id'],
            trail['city_name'],
            trail['state_id'],
        )
        cur = conn.cursor()
        cur.execute(sql, trail_data)
        conn.commit()
    return state

def add_state_ids(conn):
    state_ids = open("state_ids.json", "r").read()
    data = json.loads(state_ids)

    for state in data:
            sql = ''' INSERT OR IGNORE INTO state(
                name, id
            )
            VALUES(?,?) '''

            state_data = (state.lower().replace(' ', ''), data[state])
            input(state_data)
            cur = conn.cursor()
            cur.execute(sql, state_data)
            conn.commit()
    
if __name__ == '__main__':
    conn = main()
    if conn:
        state_trails = os.listdir("trails")
        state_ids = {}
        for state in state_trails:
            state_id = add_state_trails(conn, state)
            print(state_id)
            state_ids[state_id[0]] = state_id[1]

        # print(state_ids)

        # add_state_ids(conn)
