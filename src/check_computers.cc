/**
 * Copyright (C) 2012 Jakub Jirasek <xjiras02@stud.fit.vutbr.cz>
 *                    Libor Polčák <ipolcak@fit.vutbr.cz>
 * 
 * This file is part of pcf - PC fingerprinter.
 *
 * Pcf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pcf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pcf. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <stdbool.h>
#include <string.h>

#include "check_computers.h"
#include "computer_info.h"
#include "clock_skew.h"


#define MY_ENCODING "UTF-8"



/**
 * Make the main structure of the document
 * @param[in] filename Filename
 * @return 0 if ok
 */
int first_computer(const char *filename);

/**
 * Conversts time to its string representation in human readable format
 * @param[out] buffer Pre-allocated buffer where the output is stored
 * @param[in] buffer_size Size of the buffer
 * @param[in] time Unix time to be converted
 */
void time_to_str(char *buffer, size_t buffer_size, time_t time);

int first_computer(const char *filename)
{
  int rc;
  xmlTextWriterPtr writer;
  xmlDocPtr doc;
  
  writer = xmlNewTextWriterDoc(&doc, 0);
  if (writer == NULL)
    return(1);

  /// Default header
  rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
  if (rc < 0)
    return(1);

  /// Root node <computers>
  rc = xmlTextWriterStartElement(writer, BAD_CAST "computers");
  if (rc < 0)
    return(1);
  
  /// </computers>
  rc = xmlTextWriterEndDocument(writer);
  if (rc < 0)
    return(1);

  /// Save
  xmlFreeTextWriter(writer);
  xmlSaveFileEnc(filename, doc, MY_ENCODING);
  xmlFreeDoc(doc);
  
  return(0);
}

bool find_computer_in_saved(double referenced_skew, identity_container &identities, const double THRESHOLD, const char *database)
{
  /// No computers
  if (access(database, F_OK) != 0) {
    return false;
  }

  xmlDoc *xml_doc = xmlReadFile(database, NULL, 0);

  if (xml_doc == NULL) {
    return(false);
  }

  xmlNode *root_element = xmlDocGetRootElement(xml_doc);

  for(xmlNode *computer_node = root_element->children; computer_node != NULL; computer_node = computer_node->next) {

    if (computer_node->type != XML_ELEMENT_NODE) {
      continue;
    }

    if (strcmp((char *) computer_node->name, "computer") != 0) {
      continue;
    }

    xmlChar *xml_skew_prop = xmlGetProp(computer_node, BAD_CAST "skew");
    if (xml_skew_prop != NULL) {
      double skew_xml = atof((char *) xml_skew_prop);

      /// Comparison
      double skew_diff = fabs(referenced_skew - skew_xml);
      if (skew_diff < THRESHOLD) {
        bool name_reached = false;
        xmlNode *computer_child_node = computer_node->children;

        while (!name_reached && computer_child_node != NULL) {
          if (computer_child_node->type != XML_ELEMENT_NODE) {
            continue;
          }

          if (strcmp((char *) computer_child_node->name, "name") == 0) {
            xmlChar *computer_name  = xmlNodeGetContent(computer_child_node);
            if (computer_name != NULL) {
              identities.insert((char *) computer_name);
              name_reached = true;
              xmlFree(computer_name);
            }
          }
        }
      }
      xmlFree(xml_skew_prop);
    }
  }

  xmlFreeDoc(xml_doc);

  return true;
}

int save_active(const std::list<computer_info *> &all_computers, const char *active, computer_info_list &computers)
{
  xmlDocPtr doc;
  xmlNodePtr nodeptr = NULL, node = NULL, node_child = NULL;

  /// File doesn't exist yet
  if (first_computer(active) != 0) {
    fprintf(stderr, "Cannot create XML file: %s\n", active);
    return(1);
  }
    
  doc = xmlParseFile(active);
  if (doc == NULL ) {
    fprintf(stderr, "XML document not parsed successfully: %s\n", active);
    return(1);
  }
  
  nodeptr = xmlDocGetRootElement(doc);
  if (nodeptr == NULL) {
    xmlFreeDoc(doc); 
    return(1);
  }
  
  /// Check root (<computers>)
  if (xmlStrcmp(nodeptr->name, (const xmlChar *) "computers")) {
    fprintf(stderr, "XML document of the wrong type: %s\n", active);
    xmlFreeDoc(doc);
    return(1);
  }
  
  for (auto it = all_computers.begin(); it != all_computers.end(); ++it) {
    
    if ((*it)->get_freq() == 0) {
#ifdef DEBUG
      fprintf(stderr, "XML: skipping %s - frequency is 0\n", (*it)->get_address().c_str());
#endif
      continue;
    }
  
    /// <computer skew>
    node = xmlNewNode(NULL, BAD_CAST "computer");
    char tmp[30];
    sprintf(tmp, "%lf", (*it)->get_skew().alpha);
    xmlNewProp(node, BAD_CAST "skew", BAD_CAST tmp);
    xmlAddChild(nodeptr , node);

    // find computers with similar clock skew
    identity_container similar_skew = computers.get_similar_identities((*it)->get_address());
    for (auto skew_it = similar_skew.begin(); skew_it != similar_skew.end(); ++skew_it) {
      /// <identity>
      node_child = xmlNewNode(NULL, BAD_CAST "identity");
      /// <name>
      xmlNewChild(node_child, NULL, BAD_CAST "name", BAD_CAST skew_it->c_str());
      // Add to tree
      xmlAddChild(node, node_child);
    }

    /// <address>
    node_child = xmlNewChild(node, NULL, BAD_CAST "address", BAD_CAST (*it)->get_address().c_str());
    xmlAddChild(node, node_child);
    xmlAddChild(nodeptr, node);
    
    /// <freq>
    sprintf(tmp, "%d", (*it)->get_freq());
    node_child = xmlNewChild(node, NULL, BAD_CAST "frequency", BAD_CAST tmp);
    xmlAddChild(node, node_child);
    xmlAddChild(nodeptr, node);
    
    /// <packets>
    sprintf(tmp, "%ld", (*it)->get_packets_count());
    node_child = xmlNewChild(node, NULL, BAD_CAST "packets", BAD_CAST tmp);
    xmlAddChild(node, node_child);
    xmlAddChild(nodeptr, node);
    
    /// <date>
    const time_t last = (*it)->get_last_packet_time();
    node_child = xmlNewChild(node, NULL, BAD_CAST "date", BAD_CAST ctime(&last));
    xmlAddChild(node, node_child);
    xmlAddChild(nodeptr, node);
    
    /// Save
    xmlSaveFileEnc(active, doc, MY_ENCODING);

#ifdef DEBUG
    fprintf(stderr, "XML: saved %s: frequency %d, skew %lf\n", (*it)->get_address().c_str(), (*it)->get_freq(), (*it)->get_skew().alpha);
#endif
  }
  
  xmlFreeDoc(doc);
  
  return(0);
}
