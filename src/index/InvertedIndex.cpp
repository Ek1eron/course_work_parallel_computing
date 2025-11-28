#include "InvertedIndex.h"
#include "../utils/FileLoader.h"
#include "../utils/Tokenizer.h"
#include "../threading/ThreadPool.h"

#include <chrono>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <iterator>

static std::string toLowerCopy(std::string s)
{
    for (char& c : s)
    {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

void InvertedIndex::addDocument(int docId, const std::string& path)
{
    documents_[docId] = path;
}

void InvertedIndex::build(int numThreads)
{
    index_.clear();

    auto start = std::chrono::high_resolution_clock::now();

    if (numThreads <= 1)
    {
        for (const auto& [docId, path] : documents_)
        {
            std::string text = FileLoader::readAll(path);
            auto words = Tokenizer::tokenize(text);

            for (const auto& w : words)
            {
                index_.add(w, docId);
            }
        }
    }
    else
    {
        ThreadPool pool(numThreads);

        for (const auto& [docId, path] : documents_)
        {
            pool.enqueue([this, docId, path]()
            {
                std::string text = FileLoader::readAll(path);
                auto words = Tokenizer::tokenize(text);

                for (const auto& w : words)
                {
                    index_.add(w, docId);
                }
            });
        }

    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();

    std::cout << "[Index N=" << numThreads << "] Build time: "<<duration << " seconds.\n";
}

std::vector<int> InvertedIndex::search(const std::string& word) const
{
    std::string key = toLowerCopy(word);
    return index_.get(key);
}

std::vector<int> InvertedIndex::searchPhrase(const std::string& phrase) const {
    std::vector<int> result;

    auto words = Tokenizer::tokenize(phrase);
    if (words.empty())
    {
        return result;
    }
    std::vector<int> candidates = search(words[0]);
    std::sort(candidates.begin(), candidates.end());

    for (size_t i = 1; i < words.size(); ++i)
    {
        auto nextDocs = search(words[i]);
        std::sort(nextDocs.begin(), nextDocs.end());

        std::vector<int> inter;
        std::set_intersection(
            candidates.begin(), candidates.end(),
            nextDocs.begin(), nextDocs.end(),
            std::back_inserter(inter)
        );
        candidates.swap(inter);

        if (candidates.empty())
        {
            return result;
        }
    }

    std::string phraseLower = toLowerCopy(phrase);

    for (int id : candidates)
        {
        const std::string& path = documents_.at(id);

        std::string content;
        try
        {
            content = FileLoader::readAll(path);
        }
        catch (...)
        {
            continue;
        }

        std::string contentLower = toLowerCopy(content);

        if (contentLower.find(phraseLower) != std::string::npos)
        {
            result.push_back(id);
        }
    }

    return result;
}

const std::string& InvertedIndex::docPath(int docId) const
{
    return documents_.at(docId);
}

int InvertedIndex::totalDocuments() const
{
    return static_cast<int>(documents_.size());
}

int InvertedIndex::totalWords() const
{
    return static_cast<int>(index_.size());
}
