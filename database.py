#!/usr/bin/python
# -*- coding: utf-8 -*-

print( "Content-type:text/html\r\n\r\n")
print('<html>')

import _mysql

class Database():
    #Class used to interact with the database

    def __init__(self):
        #Tries to get connection
        try:
            self.connection = _mysql.connect(host="iesc-s2.mit.edu",user="aladetan_matiash",passwd="TYZfuR5p",db="aladetan_matiash")
            #print(self.connection) #Used to Debug
            
            
        except _mysql.Error as  e:
            print("ERROR! %d: %s" % (e.args[0],e.args[1]))

    def find_songs(self,username):
        '''
        Returns all songs in the songs table that have a specific username
        '''
        query = ("SELECT song_title FROM songs WHERE username='%s'" % username)
        self.connection.query(query)
        result = self.connection.store_result()
        rows = result.fetch_row(maxrows=0,how=0)
        # print("These are the songs that we've found for " + username + ":")
        # for row in rows:
        #     print(row)
        return rows
            
    def add_to_songs(self,username,song_title,chords):
        '''
        Inserts data into the songs table
        '''
        query = ("INSERT INTO songs (id,username,song_title,chords) VALUES (%d,'%s','%s','%s')" % (0,username,song_title,chords)) #I made the ID zero because then we don't need to manually increment, it will automatically do it for us [SQL MAGICCCC!]
        self.connection.query(query)
        self.connection.commit()

    def add_to_chords(self,chord,chord_pattern):
        '''
        Inserts data into the chords table
        '''
        query = ("INSERT INTO chords (id,chord,chord_pattern) VALUES (%d,'%s','%s')" % (0,chord,chord_pattern))
        self.connection.query(query)
        self.connection.commit()

    def print_table(self,table):
        '''
        Prints all contents of a specific table
        '''
        query = ("select * from %s" % table)
        self.connection.query(query)
        result = self.connection.store_result()
        rows = result.fetch_row(maxrows=0,how=0)
        for row in rows:
            print(row)

    def reset_table(self,table):
        '''
        Deletes everything from a specific table
        '''
        query = ("DELETE from %s" % table)
        self.connection.query(query)
        self.connection.commit()

    def get_song_chords(self, username):
        '''
        Returns a dictionary of the user's songs mapped to their respective chords
        '''
        #Dictionary that hold all of the song name and their respective chords
        self.songs_to_chords = {}
        query = ("SELECT song_title, chords FROM songs WHERE username = '%s'" % username)
        self.connection.query(query)
        result = self.connection.store_result()
        rows = result.fetch_row(maxrows=0,how=0)
        for row in rows:
            if row[0] not in self.songs_to_chords:
                self.songs_to_chords[row[0].decode("utf-8")]=row[1].decode("utf-8")
        return self.songs_to_chords
    
    def get_chord_pattern(self,chord):
        query = ("SELECT chord_pattern FROM chords WHERE chord = '%s'" % chord)
        self.connection.query(query)
        result = self.connection.store_result()
        rows = result.fetch_row(maxrows=0,how=0)
        return rows[0][0].decode("utf-8")
        
        
    def send_to_request(self,pattern):
        query = ("INSERT INTO requests (id,code) VALUES (%d,'%s')" % (0,pattern))
        self.connection.query(query)

    def get_latest_request(self):
        query = ("SELECT * FROM requests ORDER BY id DESC LIMIT 1;")
        self.connection.query(query)
        result = self.connection.store_result()
        row = result.fetch_row(maxrows=0,how=0)   
        return [row[0][1].decode("utf-8"),row[0][2]]

    def remove_song(self,username, song_title):
        query = ("DELETE FROM songs WHERE username='%s' AND song_title='%s'" % (username,song_title))
        self.connection.query(query)
        
        
#MAKE SURE TO CLOSE THE CONNECTION

#Testing the database
if __name__ == "__main__":

    database = Database()
#adding in chords

    # database.add_to_chords("C","0003")
    # database.add_to_chords("G","0232")
    # database.add_to_chords("F","2010")
    # database.add_to_chords("D","2220")
    # database.add_to_chords("Am","2000")    
    # database.add_to_chords("A","2100")
    # database.add_to_chords("Dm","2210")
    # database.add_to_chords("Bb","3211")
    # database.add_to_chords("D7","2223")
    # database.add_to_chords("G7","0212")
    # database.add_to_chords("Em","0432")
    # database.add_to_chords("E7","1202")
    # database.add_to_chords("A7","0100")
    # database.add_to_chords("Bm","4222")
    # database.add_to_chords("C7","0001")
    # database.add_to_chords("B","4322")
    # database.add_to_chords("E","4442")
    # database.add_to_chords("Eb","0331")
    # database.add_to_chords("Fm","1013")
    # database.add_to_chords("Gm","0231")


    database.print_table("requests")

#Testing get_chord_pattern method
#    print("Here is the chord A!")
#    print(database.get_chord_pattern("A"))
#    print("Here is the chord Am!")
#    print(database.get_chord_pattern("Am"))
    database.connection.close()
    print("</html>")


