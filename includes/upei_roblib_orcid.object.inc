<?php
/**
 * @file
 * Islandora object management.
 */

/**
 * Updates Islandora Person object MADS with an ORCID iD.
 *
 * @param array $data
 *   The ORCID iD and tokens.
 * @param string $pid
 *   The PID of the islandora object to update.
 *
 * @throws \Exception
 *   If the Object does not exist, the MADS can't be found or an iD exists in
 * the object.
 */
function upei_roblib_orcid_update_object($data, $pid) {
  if (empty($pid)) {
    throw new Exception('Person Entity Object not found');
  }
  $orcid_id = isset($data['orcid']) ? $data['orcid'] : '';
  if (empty($orcid_id)) {
    throw new Exception('ORCID iD not found in response');
  }
  $profile_object = islandora_object_load($pid);
  if (empty($profile_object)) {
    throw new Exception('Could not load Person Entity Object');
  }
  $mads = $profile_object['MADS']->content;
  if (empty($mads)) {
    throw new Exception('MADS datastream not found');
  }
  $updated_xml = upei_roblib_orcid_update_mads($mads, $orcid_id);
  $profile_object['MADS']->setContentFromString($updated_xml);
  upei_roblib_orcid_add_orcid_ds($data, $profile_object);
  upei_roblib_orcid_update_xacml($profile_object);
}

/**
 * Add the ORCID datastream.
 *
 * @param array $data
 *   The data received from ORCID including iD and tokens
 * @param FedoraObject $object
 *   The object to add the datastream.
 *
 * @throws \Exception
 *   If the data array cannot be converted to json.
 */
function upei_roblib_orcid_add_orcid_ds($data, $object) {
  $dsid = 'ORCID';
  $json = json_encode($data);
  if (empty($json)) {
    throw new Exception('Could not parse ORCID data');
  }
  $datastream = isset($object[$dsid]) ? $object[$dsid] : $object->constructDatastream($dsid);
  $datastream->label = 'ORCID';
  $datastream->mimeType = 'application/json';
  $datastream->setContentFromString($json);
  if (!isset($object['DSID'])) {
    $object->ingestDatastream($datastream);
  }
}

/**
 * Adds an XACML rule to only allow the logged in user access to the ORCID ds.
 *
 * @param FedoraObject $object
 *   The object to add the rule.
 */
function upei_roblib_orcid_update_xacml($object) {
  global $user;
  $existing_xacml = isset($object['XACML']->content) ? $object['XACML'] : NULL;
  $xacml = new IslandoraXacml($object, $existing_xacml);
  $xacml->datastreamRule->addDsid(array('ORCID'));
  $xacml->datastreamRule->addUser(array($user->name));
  $xacml->writeBackToFedora();
}

/**
 * Adds an ORCID iD to the MADS xml.
 *
 * @param string $mads_xml
 *   The MADS xml.
 * @param string $orcid_id
 *   An ORCID iD.
 *
 * @return string
 *   The updated MADS xml.
 * @throws \Exception
 *   If the MADS cannot be loaded as xml or the MADS already has an ORCID iD.
 */
function upei_roblib_orcid_update_mads($mads_xml, $orcid_id) {
  $xml = new DOMDocument();
  $test = $xml->loadXML($mads_xml);
  if (empty($test)) {
    throw new Exception("Error parsing MADS xml");
  }
  $query = "//mads:identifier[@type='orcid']";
  $xpath = new DOMXPath($xml);
  $xpath->registerNamespace('mads', 'http://www.loc.gov/mads/v2');
  $results = $xpath->query($query, $xml);
  $found = (bool) $results->length;
  if (!empty($found)) {
    throw new Exception('ORCID ID Exists');
  }
  $root = $xml->documentElement;
  $identifier = $xml->createElementNS("http://www.loc.gov/mads/v2",
    "mads:identifier", $orcid_id);
  $attr = $xml->createAttribute('type');
  $attr->value = 'orcid';
  $identifier->appendChild($attr);
  $root->appendChild($identifier);
  return $xml->saveXML();
}
