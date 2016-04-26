#!/usr/bin/python
# -*- coding: utf-8 -*-

# print( "Content-type:text/html\r\n\r\n")
# print('<html>')

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
        Inserts data into the songs table, where data is a list of values specific to that table
        '''
        query = ("INSERT INTO songs (id,username,song_title,chords) VALUES (%d,'%s','%s','%s')" % (0,username,song_title,chords)) #I made the ID zero because then we don't need to manually increment, it will automatically do it for us [SQL MAGICCCC!]
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

    def remove_song(self,username, song_title):
        query = ("DELETE FROM songs WHERE username='%s' AND song_title='%s'" % (username,song_title))
        self.connection.query(query)
        
        
#MAKE SURE TO CLOSE THE CONNECTION

#Testing the database
# if __name__ == "__main__":
#     database = Database()
#     database.reset_table("songs")
#     database.add_to_songs(1,"Matias","The Song","A B C D")
#     print("Dictionary before removing \'The Song\'")
#     print(database.get_song_chords("Matias"))
#     print("Database before removing the song:")
#     database.print_table("songs")
#     database.remove_song("Matias","The Song")
#     print("Dictionary after removing \'The Song\'")
#     print(database.get_song_chords("Matias"))
#     print("Database after removing the song:")
#     database.print_table("songs")
#     database.connection.close() #KEEP THIS
#     print("</html>")


