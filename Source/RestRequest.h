/*
  ==============================================================================

    RestRequest.h
    Created: 12 Jan 2021 7:21:13pm
    Author:  Rodolfo Ortiz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class RestRequest
{
public:
    
    RestRequest (String urlString) : url (urlString) {}
    RestRequest (URL url)          : url (url) {}
    RestRequest () {}
    
    struct Response
    {
        Result result;
        StringPairArray headers;
        var body;
        String bodyAsString;
        int status;
        
        Response() : result (Result::ok()), status (0) {} // not sure about using Result if we have to initialise it to ok...
    } response;

    
    RestRequest::Response execute ()
    {
        auto urlRequest = URL(endpoint);
        bool hasFields = (fields.getProperties().size() > 0);
        if (hasFields)
        {
            MemoryOutputStream output;

            fields.writeAsJSON (output, 0, false, 20);
            urlRequest = urlRequest.withPOSTData (output.toString());
//            DBG(urlRequest.getPostData());
        }
        
        std::unique_ptr<InputStream> input (urlRequest.createInputStream (hasFields, nullptr, nullptr, stringPairArrayToHeaderString(headers), 3000, &response.headers, &response.status, 5, verb));
        
        response.result = checkInputStream(input);
        if (response.result.failed()) return response;

        response.bodyAsString = input->readEntireStreamAsString();
        response.result = JSON::parse(response.bodyAsString, response.body);

        return response;
    }
 
    
    RestRequest get (const String& endpoint)
    {
        RestRequest req (*this);
        req.verb = "GET";
        req.endpoint = endpoint;
        
        return req;
    }

    RestRequest post (const String& endpoint)
    {
        RestRequest req (*this);
        req.verb = "POST";
        req.endpoint = endpoint;
        
        return req;
    }
    
    RestRequest put (const String& endpoint)
    {
        RestRequest req (*this);
        req.verb = "PUT";
        req.endpoint = endpoint;
        
        return req;
    }

    RestRequest del (const String& endpoint)
    {
        RestRequest req (*this);
        req.verb = "DELETE";
        req.endpoint = endpoint;
        
        return req;
    }
    
    RestRequest field (const String& name, const var& value)
    {
        fields.setProperty(name, value);
        return *this;
    }
    
    RestRequest header (const String& name, const String& value)
    {
        RestRequest req (*this);
        headers.set (name, value);
        return req;
    }
    
    const URL& getURL() const
    {
        return url;
    }
    
    const String& getBodyAsString() const
    {
        return bodyAsString;
    }

    //JUCE NetworkingDemo.h example
    String getResultText (const URL& url)
    {
        StringPairArray responseHeaders;
        int statusCode = 0;

        if (auto stream = std::unique_ptr<InputStream> (url.createInputStream (false, nullptr, nullptr, {},
                                                                               10000, // timeout in millisecs
                                                                               &responseHeaders, &statusCode)))
        {
            DBG(responseHeaders.getDescription());
            DBG(stream->readEntireStreamAsString());
            return (statusCode != 0 ? "Status code: " + String (statusCode) + newLine : String())
                    + "Response headers: " + newLine
                    + responseHeaders.getDescription() + newLine
                    + "----------------------------------------------------" + newLine
                    + stream->readEntireStreamAsString();
        }

        if (statusCode != 0)
            return "Failed to connect, status code = " + String (statusCode);

        return "Failed to connect!";
    }
    
private:
    URL url;
    StringPairArray headers;
    String verb;
    String endpoint;
    DynamicObject fields;
    String bodyAsString;
    
    Result checkInputStream (std::unique_ptr<InputStream>& input)
    {
        if (!input)
            return Result::fail ("HTTP request failed, check your internet connection");
        
        return Result::ok();
    }
    
    static String stringPairArrayToHeaderString(StringPairArray stringPairArray)
    {
        String result;
        for (auto key : stringPairArray.getAllKeys())
        {
            result += key + ": " + stringPairArray.getValue(key, "") + "\n";
        }
        return result;
    }
};
