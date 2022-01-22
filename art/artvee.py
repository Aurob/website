from bs4 import BeautifulSoup
import csv
import json
import logging
import math
import os
import re
import requests

def create_json(csv_path, json_path):

    """
    Args:
        csv_path : file path for the csv
        json_path: file path for the json

    Explanation:
        Reads the csv and converts to a dictionary
        Uses json.dumps() to dump the data and write to json
    """

    data = {} 
    with open(csv_path, encoding='utf-8') as csvf: 
        csv_reader = csv.DictReader(csvf) 
          
        # Convert each row into a dictionary and add it to data 
        for rows in csv_reader: 
            key = rows['Title'] 
            data[key] = rows 

    with open(json_path, 'w', encoding='utf-8') as jsonf: 
        jsonf.write(json.dumps(data, indent=4)) 


def scrape_meta_images(url, category, data_path, writer):

    """
    Args:  
        url (str): URL for the paginated category pages
        category (str): The category used in the url
        data_path (str): The path where the csv, json, and temporary images will be stored
        writer: Writes the appended elements in data to the csv

    Explanation:
        Parses the page of 70 artworks and puts cards, which contain the image and metadata, in a list
        Parses the page for the image download page to be passed in after scraping metadata
        In each card, finds the title and artist and appends to data []
        Scrapes the image and uploads it
        Writes data to the csv and moves to the next card
    """

    page = requests.get(url)
    soup = BeautifulSoup(page.content, "html.parser")
    cards = soup.find_all("div", class_="product-wrapper snax")
    for card in cards:
        data = []
        #Formatted in nested if-statements to prevent receiving an error for a missing element/class (None type)
        title = card.find("div", class_="linko")
        if (title != None):
            if (title.find("a") != None):
                title = title.get_text()
                data.append(title)
        else:
            title = "Untitled"
            data.append(title)

        artist_info = card.find("div", class_="woodmart-product-brands-links")
        if (artist_info != None):
            artist_info = artist_info.get_text()
            data.append(artist_info)
        else:
            artist_info = "Unknown"
            data.append(artist_info)

        art_link = card.find("img", class_="wp-post-image")
        if (art_link != None):
            art_link = art_link["src"].replace("/ft/", "/sftb/")
            data.append(art_link)
        else:
            data.append("None")
        
        data.append(category)
        
        writer.writerow(data)

def count_pages(category):

    """
    Args:
        category : used in the url to find the page and its respective results

    Explanation:
        Parse first page of a category
        Return the number of pages from the pager at the bottom of the result page
    """

    url = "https://artvee.com/c/%s/page/1/?per_page=70" % category
    page = requests.get(url)
    soup = BeautifulSoup(page.content, "html.parser")
    pages = soup.find_all("a", class_="page-numbers")
    last_page = int(pages[-2].getText())
    return last_page

if __name__ == "__main__":
    data_path = "./"
    csv_path = os.path.join(data_path, "artvee.csv")
    json_path = os.path.join(data_path + "artvee.json")
    if (data_path == ""):
        print("\nPlease assign a value to the data_path\n")
    else:
        with open(csv_path, "w", newline = "", encoding="utf-8") as f:
            #Create csv writer and header row
            headers = ["Title", "Artist", "Link", "Category"]
            writer = csv.writer(f)
            writer.writerow(headers)

            #Artvee categorizes its works and these are how they are written in the url
            categories = ["abstract", "figurative", "landscape", "religion", "mythology", "posters", "animals", "illustration", "still-life", "botanical", "drawings", "asian-art"] #abstract"] #, "

            for category in categories:
                no_pages = count_pages(category)
                print(no_pages)
                #Pagination
                for p in range(1, no_pages + 1):
                   print("Currently looking at: %s, page %d" % (category, p))
                   url = "https://artvee.com/c/%s/page/%d/?per_page=70" % (category, p)
                   scrape_meta_images(url, category, data_path, writer)

            f.close()

        #Create the json after all data is written in the csv and upload it to s3 bucket
        # create_json(csv_path, json_path)
