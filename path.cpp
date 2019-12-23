#include "path.h"

#include <vector>

namespace asio_httpd {

Path::Path()
    {}

Path::Path(const std::string& path)
    {
    set(path);
    }

Path::~Path()
    {}

void Path::set(const std::string& path)
    {
    std::string currentcomponent;
    for(std::string::const_iterator i = path.begin() ; i != path.end(); ++i)
        {
        if(*i != '/')
            currentcomponent.append(*i, 1);
        else
            {
            if(m_components.empty() || m_components.at(m_components.size() - 1) != "/")
                m_components.push_back(std::string("/"));
            if(!currentcomponent.empty())
                {
                if(currentcomponent != ".." && currentcomponent != ".")
                    {
                    m_components.push_back(currentcomponent);
                    currentcomponent.clear();
                    }
                else if(currentcomponent == "..")
                    {
                    if(m_components.size() >= 2)
                        {
                        // Remove the last / and the path element before that
                        m_components.pop_back();
                        m_components.pop_back();
                        }
                    else
                        throw MalformedPath("Path contains .. component leading past the root");
                    }
                // When currentcomponent == ".", do nothing
                }
            }
        }

    if(!currentcomponent.empty())
        {
        if(currentcomponent != ".." && currentcomponent != ".")
            {
            m_components.push_back(currentcomponent);
            currentcomponent.clear();
            }
        else if(currentcomponent == "..")
            {
            if(m_components.size() >= 2)
                {
                // Remove the last / and the path element before that
                m_components.pop_back();
                m_components.pop_back();
                }
            else
                throw MalformedPath("Path contains .. component leading past the root");
            }
        }
    }

void Path::append(const Path& path)
    {
    std::vector<std::string>::const_iterator it = path.m_components.begin();

    // Make sure the path to append does not start with "/"
    // (Paths stored in Path objects are always normalized and never contain consecutive slashes)
    if(*it == "/")
        it++;

    // Make sure this path does end with / before appending
    if(!m_components.empty() && m_components.at(m_components.size() - 1) != "/")
        m_components.push_back(std::string("/"));

    m_components.insert(m_components.end(), it, path.m_components.end());
    }

bool Path::operator==(const Path& path) const
    {
    if(num_elements() != path.num_elements())
        return false;

    // At this point, both paths have the same number of elements
    return std::equal(m_components.begin(), m_components.end(), path.m_components.begin());
    }

bool Path::operator<(const Path& path) const
    {
    if(m_components.size() < path.m_components.size())
        return true;
    if(m_components.size() > path.m_components.size())
        return false;

    std::vector<std::string>::const_iterator it1 = m_components.begin();
    std::vector<std::string>::const_iterator it2 = path.m_components.begin();

    while(it1 != m_components.end())
        {
        if(*it1 < *it2)
            return true;
        if(*it1 > *it2)
            return false;

        ++it1; ++it2;
        }

    return false;
    }

std::string Path::str() const
    {
    std::string out;
    for(std::vector<std::string>::const_iterator i = m_components.begin() ; i != m_components.end() ; ++i)
        out += *i;
    return out;
    }

bool Path::is_in_subtree(const Path& path) const
    {
    if(path.m_components.size() < m_components.size())
        return false;

    std::vector<std::string>::const_iterator it = path.m_components.begin();
    for(std::vector<std::string>::const_iterator i = m_components.begin() ; i != m_components.end() ; ++i)
        {
        if(*i != *it)
            return false;
        ++it;
        }
    return true;
    }

unsigned int Path::num_elements() const
    {
    return m_components.size();
    }

bool Path::is_absolute() const
    {
    return !m_components.empty() && m_components.at(0) == "/";
    }

Path Path::difference(const Path& path) const
    {
    if(is_in_subtree(path))
        {
        Path out;
        std::vector<std::string>::const_iterator it = path.m_components.begin();
        it += m_components.size();
        out.m_components.insert(out.m_components.end(), it, path.m_components.end());
        return out;
        }
    else
        throw NotInSubtree("Path " + path.str() + " is not in subtree of " + str());
    }

std::ostream& operator<<(std::ostream& out, const Path& p)
    {
    out << p.str();
    return out;
    }

}
