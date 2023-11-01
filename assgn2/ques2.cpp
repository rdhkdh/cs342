#include <iostream>
#include <string>
#include <map>
#include <list>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

using namespace std;

const int CACHE_SIZE = 5;

struct CachedWebPage
{
    string url;
    string content;
};

class WebPageCache
{
private:
    map<string, CachedWebPage> cacheMap;
    list<string> accessOrderList;

public:
    // Constructor
    WebPageCache()
    {
        cacheMap.clear();
        accessOrderList.clear();
    }

    // Fetch a web page from the cache or the internet
    string retrievePage(const string &webUrl)
    {
        if (cacheMap.find(webUrl) != cacheMap.end())
        {
            // Page is in cache, move it to the front of accessOrderList
            accessOrderList.remove(webUrl);
            accessOrderList.push_front(webUrl);
            return cacheMap[webUrl].content;
        }
        else
        {
            // Page is not in cache, fetch it from the internet
            string webContent = fetchFromInternet(webUrl);

            // If the cache is full, evict the least recently used page
            if (cacheMap.size() >= CACHE_SIZE)
            {
                string lruPageUrl = accessOrderList.back();
                cacheMap.erase(lruPageUrl);
                accessOrderList.pop_back();
            }

            // Add the new page to the cache
            cacheMap[webUrl] = {webUrl, webContent};
            accessOrderList.push_front(webUrl);
            return webContent;
        }
    }

    // Display the contents of the cache
    void showCache()
    {
        for (const auto &webUrl : accessOrderList)
        {
            cout << webUrl << " (accessed) -> ";
        }
        cout << "END" << endl;
    }

private:
    // Simulate an HTTP GET request to fetch a web page
    string fetchFromInternet(const string &webUrl)
    {
        // Extract the host and path from the URL
        size_t httpStartPos = webUrl.find("http://");
        if (httpStartPos == string::npos)
        {
            cerr << "Invalid URL format. It should start with 'http://'" << endl;
            return "";
        }

        size_t hostStartPos = httpStartPos + 7; // Skip "http://"
        size_t pathStartPos = webUrl.find('/', hostStartPos);

        string hostName = webUrl.substr(hostStartPos, pathStartPos - hostStartPos);
        string webPath = (pathStartPos != string::npos) ? webUrl.substr(pathStartPos) : "/";

        // Create a socket to connect to the web server
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            cerr << "Error opening socket" << endl;
            return "";
        }

        struct sockaddr_in serverAddress;
        struct hostent *serverInfo;

        serverInfo = gethostbyname(hostName.c_str());
        if (serverInfo == NULL)
        {
            cerr << "Error, no such host" << endl;
            close(sockfd);
            return "";
        }

        bzero((char *)&serverAddress, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        bcopy((char *)serverInfo->h_addr, (char *)&serverAddress.sin_addr.s_addr, serverInfo->h_length);
        serverAddress.sin_port = htons(80); // HTTP port

        if (connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        {
            cerr << "Error connecting to server" << endl;
            close(sockfd);
            return "";
        }

        // Send an HTTP GET request
        string httpRequest = "GET " + webPath + " HTTP/1.1\r\n"
                                                "Host: " +
                             hostName + "\r\n"
                                        "Connection: close\r\n"
                                        "\r\n";

        if (send(sockfd, httpRequest.c_str(), httpRequest.length(), 0) < 0)
        {
            cerr << "Error sending request" << endl;
            close(sockfd);
            return "";
        }

        // Receive the HTTP response
        string httpResponse;
        char buffer[1024];
        int bytesRead;
        while ((bytesRead = recv(sockfd, buffer, sizeof(buffer), 0)) > 0)
        {
            httpResponse.append(buffer, bytesRead);
        }

        close(sockfd);

        // Find the start of the content (after headers)
        size_t contentStartPos = httpResponse.find("\r\n\r\n");
        if (contentStartPos != string::npos)
        {
            return httpResponse.substr(contentStartPos + 4);
        }

        return "";
    }
};

int main()
{
    WebPageCache webpageCache;

    while (true)
    {
        string inputUrl;
        cout << "Enter a URL to fetch (or 'exit' to quit): ";
        cin >> inputUrl;

        if (inputUrl == "exit")
        {
            break;
        }

        string pageContent = webpageCache.retrievePage(inputUrl);
        cout << "Content: " << pageContent << endl;

        cout << "Cache status: ";
        webpageCache.showCache();
    }

    return 0;
}
