<?php

// $Id$

class DHiVideoPlayer {

  function __construct($pid = '') {
    /*module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    module_load_include('inc', 'fedora_repository', 'CollectionClass');*/
   
   if (!empty($pid)) {
    	  $this->pid = $pid;
      	//$this->item = new Fedora_Item($pid);
    }
    
    module_load_include('object.inc', 'islandora_fedora_api');
    
  }

  function showVideoPlayer() {
    global $base_url;
    $path = drupal_get_path('module', 'dhi_videoplayer');
    $fullPath = $path . '/';

	//$viewer_url = $base_url . '/' . $fullPath . 'plugins/BookReaderDemo/index.php?pid=' . $this->pid . '&label=' . $this->item->objectProfile->objLabel;
    //$html = '<iframe src="' . $viewer_url . '" scrolling="0" frameborder="0" style="width: 100%; height: 800px;">Errors: unable to load viewer</iframe>';

// STARTS HERE --------	
	
	//get info for query
  $query_file_name = drupal_get_path('module', 'dhi_videoplayer') . '/lowres_query.txt';
  $query_file_handle = fopen($query_file_name, "r");
  $query = fread($query_file_handle, filesize($query_file_name));
  fclose($query_file_handle);
  $limit = -1;
  $offset = 0 ;
  $type = 'sparql';
  $query_url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');

  //inject collection value into the query
  $query = preg_replace('/\$collection_object/', '<info:fedora/' . $this->pid . '>', $query);

  //run query
  $query_url .= "?type=tuples&flush=TRUE&format=Sparql" . (($limit > 0)?("&limit=$limit"):("")) . "&offset=$offset&lang=$type&query=" . urlencode($query);
  $query_return_string = do_curl($query_url);
  if ($query_return_string) {
    $query_return_dom = DOMDocument::loadXML($query_return_string);
  }
  
  //format query results for use (xml->array)
  $result_nodes = $query_return_dom->getElementsByTagName('result');
  $result_nodes_length = $result_nodes->length;
  
  // Add javascript here
  drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/jquery-1.5.2.min.js');
  drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/jquery.lightbox-0.5.min.js');
  
  drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/video.js');
  drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/popcorn/popcorn.js');
  drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/popcorn/plugins/footnote/popcorn.footnote.js');
  drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/popcorn/plugins/transcript/popcorn.transcript.js');
  drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/parsers/parserXML/popcorn.parseXML.js');
  
  drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/popcorn/config.js');
  drupal_add_js(drupal_get_path('module', 'dhi_videoplayer') . '/js/dhi.transcriptHighlight.js');
  
  
  // DELETE ME, TESTING PURPOSES ONLY
  
  $html = "<p><strong>Query:</strong><br /><pre>".$query."</pre></p>\n";
  $html .= "<p><strong>Results: </strong> {$result_nodes_length} Results</p>\n"; 
  
	if ($result_nodes_length > 0) {
		for ($i=0; $i < $result_nodes_length; $i++) {
			$result_node = $result_nodes->item($i);
			$result_values = $result_node->childNodes;
			
			$label_element = $result_node->getElementsByTagName('label')->item(0);
			$clip_index_element = $result_node->getElementsByTagName('clip_index')->item(0);
			$subbed_clip_element = $result_node->getElementsByTagName('subbed_clip')->item(0);
			$transcript_element = $result_node->getElementsByTagName('transcript')->item(0);
			
			$subbed_clip_object = $subbed_clip_element->getAttribute('uri');
			$subbed_clip_object = substr($subbed_clip_object, 12, strlen($subbed_clip_object));//need to extract the pid
			
			$transcript_object = $transcript_element->getAttribute('uri');
			$transcript_object = substr($transcript_object, 12, strlen($transcript_object)); // extract PID
			
			$subbed_clip_data[$subbed_clip_object] = array();
			
			if ($label_element) {
				$label = $label_element->nodeValue;
				if ($label) {//the label may not be pupulated
					$subbed_clip_data[$subbed_clip_object]['label'] = $label;
				}
			}
			
			if ($clip_index_element) {
				$clip_index = $clip_index_element->nodeValue;
				if ($clip_index) {
					$subbed_clip_data[$subbed_clip_object]['clip_index'] = $clip_index;
				}
			}
			
			if ($transcript_object) {
				$subbed_clip_data[$subbed_clip_object]['transcript'] = $transcript_object;
			}
			
			// DELETE ME, TESTING PURPOSES ONLY
			$html .= "<p>Result {$i}<br />
			<strong>Label:</strong> {$label}<br />
			<strong>Clip Index:</strong> {$clip_index}<br />
			<strong>URI:</strong> {$subbed_clip_object}<br />
			<strong>Transcript:</strong> {$transcript_object}</p>\n";
			
		}
		//return $subbed_clip_data;
		//return False;
		
		// DELETE ME, TESTING PURPOSES ONLY
		$html .= "<p><strong>Testing Loading Object Datastreams:</strong><br />
		".$subbed_clip_object."</p>";
		
		$subbed_clip_object = "HamiltonJapaneseComparativeFilm:91";
		$fedora_base_url = variable_get('fedora_base_url', 'http://localhost:8080/fedora');
		$access_url = $fedora_base_url . "/objects/" . $subbed_clip_object . "/datastreams/LOWRES/content";
		$html .= "<div><video id=\"video\" class=\"video-js\" width=\"320\" height=\"240\" controls=\"controls\" preload=\"auto\"><source src=\"$access_url\"></div>\n";
		//$html .= "<p>$access_url</p>\n";
		
		module_load_include('object.inc', 'islandora_fedora_api'); //for getting an object
		$object= new FedoraObject($subbed_clip_object);
		$object_datastreams=$object->get_datastreams();
		if (!isset($object_datastreams['lowres'])) {
		//return FALSE;
			$html .= "<p>Found Datastream \"LOWRES\"</p>\n";
		}
		
		// REST format
		// http://dora-dev.hpc.hamilton.edu:8080/fedora/objects/HamiltonJapaneseComparativeFilm%3A91/datastreams/LOWRES/content
		// base url: from line 44 ($query_url)
		

	}
	
	return $html;
	
  }

 /**
   * @author: William Panting
   *
   * @date: 16/05/2011
   *
   * @return tabs: the html to display, in the form of a set of tabs
   */
  function showVideoStreams()
  {
     //get the book viewer iframe
    $viewer=$this->showVideoPlayer();

    //set up tabs
    $tabs = array();
    $tabs = array('#type' => 'tabset');

    //get metadata info
    //$metadata = $this->getMetaData();

    //populate tabs
    /*
    $tabs['Description'] = array(
      '#type' => 'tabpage',
      '#title' => 'Description',
      '#content' => $metadata,
      '#weight' => 2
    );*/

    $tabs['View'] = array(
      '#type' => 'tabpage',
      '#title' => 'View',
      '#content' => $viewer,
      '#weight' => 1
    );
    //return the tabs for display
    return $tabs;
  }


 /**
   * @author: William Panting
   *
   * @date: 19/05/2011
   *
   * @return output: the html of data to display
   *
   * Small modification of UoM's exif plugin
   */
  function getMetaData()
  {
    $output='';
    $MODS = $this->item->get_datastream_dissemination('MODS');
    if (trim($MODS) != '')
    {
      $MODSDom = DOMDocument::loadXML($this->item->get_datastream_dissemination('MODS'));
      if ($MODSDom != NULL)
      {
        $description = $MODSDom->getElementsByTagName('*');
        if ($description->length > 0)
        {
          $description=$description->item(0);
          $output .= '<div class="fedora_technical_metadata"><ul>';
          for ($i=0;$i<$description->childNodes->length;$i++)
          {
            $name=$description->childNodes->item($i)->nodeName;
            $value=$description->childNodes->item($i)->nodeValue;
            if ($name != '#text'  && !preg_match('/^System\:.*$/',$name) && trim($value) != '')
            {
              list($type,$name) = preg_split('/\:/',$name);
              $name = trim(preg_replace('/(?<!^)([A-Z][a-z]|(?<=[a-z])[A-Z])/'," $1", $name));
              $output .= '<li><b>'.$name. '</b>:  '. $value .' </li>';
            }
          }
          $output.='</ul></div>';
        }
      }
    }
    return $output;
  }
}

?>