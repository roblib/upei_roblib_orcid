<?php
/**
 * @file
 * API calls to orcid and islandora.
 */


/**
 * Callback for ORCID.
 *
 * After a user signs in to ORCID and authorizes (or not) our app they get
 * redirected here.
 */
function upei_roblib_orcid_response() {
  $params = drupal_get_query_parameters();
  if (empty($params)) {
    upei_roblib_orcid_handle_orcid_error('Error communicating with ORCID', 'no response');
  }
  $code = isset($params['code']) ? $params['code'] : '';
  $error = isset($params['error']) ? $params['error'] : '';
  if (!empty($code)) {
    $response = upei_roblib_orcid_get_orcid_id($code);
    if (empty($response)) {
      upei_roblib_orcid_handle_orcid_error('Error communicating with ORCID', 'no response when retrieving id');
    }
    if ($response->code === '200') {
      upei_roblib_orcid_successful_token_request($response);
    }
    else {
      upei_roblib_orcid_failed_token_request($response);
    }
  }
  else {
    if (!empty($error)) {
      upei_roblib_orcid_handle_orcid_error($error, $params['error_description']);
    }
  }
}

/**
 * Redirects to the users home page and prints the error to the screen.
 *
 * @param string $error
 *   The error that occurred.
 * @param string $error_description
 *   A description of the error.
 */
function upei_roblib_orcid_handle_orcid_error($error, $error_description) {
  global $user;
  drupal_set_message(t('Sorry, the connection was not successful. There was an Error linking your profile with an ORCID ID.
    @error. @error_description.  Try again?',
    array(
      '@username' => $user->name,
      '@error' => $error,
      '@error_description' => $error_description,
    )), 'info');
  drupal_goto('user/' . $user->uid);
}

/**
 * After a successful token request add the ORCID iD to the MADS datastream.
 *
 * @param object $response
 *   A PHP object containing the data (body) of the response.
 */
function upei_roblib_orcid_successful_token_request($response) {
  module_load_include('inc', 'upei_roblib_orcid', 'includes/upei_roblib_orcid.object');
  $data = json_decode($response->data, TRUE);
  global $user;
  $pid = upei_roblib_orcid_get_user_entity();
  try {
    upei_roblib_orcid_update_object($data, $pid);
  }
  catch (Exception $e) {
    watchdog('UPEI Roblib Orcid',
      'There was an Error linking your Person Entity Profile with an ORCID ID. %msg, 
    for user %username and %pid',
      array(
        '%msg' => $e->getMessage(),
        '%username' => $user->name,
        '%pid' => $pid,
      ), WATCHDOG_ERROR);
    drupal_set_message(t('Sorry, the connection was not successful. There was an Error linking your profile with an ORCID ID. @msg, 
    for user @username and @pid.  Try again?',
      array(
        '@msg' => $e->getMessage(),
        '@username' => $user->name,
        '@pid' => $pid,
      )), 'info');
    drupal_goto('user/' . $user->id);
  }
  drupal_set_message(t('Thank you! Your ORCID iD (@orcid_id) is now linked to your profile (@pid).',
    array('@orcid_id' => $data['orcid'], '@pid' => $pid)));
  drupal_goto('islandora/object/' . urlencode($pid));
}

/**
 * After a failed token request redirect to the users home and print the error.
 *
 * @param object $response
 *   A PHP object containing the data (body) of the response.
 */
function upei_roblib_orcid_failed_token_request($response) {
  global $user;
  $data = json_decode($response->data, TRUE);
  drupal_set_message(t('Sorry, the connection was not successful.  Error linking your Scholar profile with your ORCID ID, @error.',
    array('@error' => $data['status_message'])), 'info');
  drupal_goto('user/' . $user->uid);
}

/**
 * API call to ORCID to exchange a code for tokens and ORCID iD.
 *
 * @param string $code
 *   A string supplied by ORCID from a previous API call
 *
 * @return object
 *   A PHP object containing the response.
 */
function upei_roblib_orcid_get_orcid_id($code) {
  $path = variable_get('upei_roblib_orcid_oauth_token_endpoint');
  $client_id = variable_get('upei_roblib_orcid_oauth_client_id');
  $client_secret = variable_get('upei_roblib_orcid_oauth_client_secret');
  $redirect_uri = url(NULL, array('absolute' => TRUE)) . UPEI_ROBLIB_ORCID_CALLBACK_URI . '/';
  $params = array(
    'client_id' => $client_id,
    'client_secret' => $client_secret,
    'grant_type' => 'authorization_code',
    'code' => $code,
    'redirect_uri' => $redirect_uri,
  );
  $data = http_build_query($params);
  $headers = array(
    'Accept' => 'application/json',
    'Content-Type' => 'application/x-www-form-urlencoded',
  );
  $options = array('headers' => $headers, 'data' => $data, 'method' => 'POST');
  return drupal_http_request($path, $options);
}
