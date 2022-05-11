#include <iostream>
#include <fstream>
#include <unordered_set>
#include <sstream>
#include <vector>
#include <curl/curl.h>

using std::cout;            using std::endl;
using std::string;          using std::unordered_set;
using std::ifstream;        using std::stringstream;
using std::ofstream;        using std::vector;

unordered_set<string> findWikiLinks(const string& page_html);
string filter(const string& page_html, int &pos);
void read2file(string url, string filename);
bool visitPage(unordered_set<string> &visited, vector<string> &path, string visiting, string target, int degree);

int main() {
    // starting wiki page
    string curr = "Movie_camera";

    // target wiki page
    string target = "Anger";

    unordered_set<string> visited;
    vector<string> path;
    visitPage(visited, path, curr, target, 5);
    for(string ea:path) {
        cout << ea << "->";
    }
    cout << endl;
    return 0;

    /* Note if your file reading isn't working, please follow the
     * steps posted on the front page of the CS106L website. Essentially
     * go to the projects tab on the panel on the left, and in the run
     * section, uncheck the "Run in terminal" box and re-check it. This
     * should fix things.
     */
}

/* 
 * Visit the page recusive ly up to n degrees to find the target page.
 */

bool visitPage(unordered_set<string> &visited, vector<string> &path, string visiting, string target, int degree) {
    path.push_back(visiting);
    visited.insert(visiting);
    
    if(visiting == target) {
        return true;
    }
    if(degree > 0){
        read2file("https://en.wikipedia.org/wiki/" + visiting, "data.txt");
        ifstream t("data.txt");
        stringstream buffer;
        buffer << t.rdbuf();
        t.close();
        unordered_set<string> result = findWikiLinks(buffer.str());
        for(string ea:result){
            if(visited.find(ea) == visited.end()) {
                if(visitPage(visited, path, ea, target, degree - 1)) {
                    return true;
                }
            }
        }
    }
    path.pop_back();
    return false;
}




/*
 * Function used for curl to write data into the file.
*/

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

void read2file(string url, string filename) {
    CURL *curl = curl_easy_init();                                                                                                                                                                                                             
    if (curl)
    {   
        FILE * fp = fopen(filename.c_str(),"wb");
        if(!fp) {
            cout << "File creation failed" << endl;
            return;
        }
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }
} 



/*
 * Return the first link obtained from it onwards.
 * "EXTRACT"
 *          ^new pointer locations
*/
string filter(string::const_iterator &page_it) {
    string output;
    int count = 0;
    while(count < 2){
        if(*page_it == '"') {
            count ++;
        }
        else if(count >0){
            if(*page_it == '#' || *page_it == ':')
                return "";
            output += *page_it;
        }
        page_it ++;
    }

    if(output.substr(0, 6) != "/wiki/") {
        return "";
    }
    return output.substr(6);
}

/*
 * Return the link obtained from a page HTML.
*/

unordered_set<string> findWikiLinks(const string& page_html) {
    string target = "href=";
    unordered_set<string> output;
    string::const_iterator it = page_html.begin();
    while((it = std::search(it, page_html.end(), target.begin(), target.end())) != page_html.end()){
        string link = filter(it);
        if(link.size() > 0) {
            output.insert(link);
        }
    }
    return output;
}
