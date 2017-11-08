<?php
/**
 * @file
 * Admin form and validation for admin form.
 */

/**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function upei_roblib_orcid_admin_form($form, &$form_state) {

  $form['upei_roblib_orcid_oauth_token_endpoint'] = array(
    '#title' => t('Token endpoint URL'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => variable_get('upei_roblib_orcid_oauth_token_endpoint', 'https://sandbox.orcid.org/oauth/token'),
    '#description' => t('A valid URL from where access tokens can be retrieved.'),
  );

  $form['upei_roblib_orcid_oauth_auth_endpoint'] = array(
    '#title' => t('Authorization endpoint URL'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => variable_get('upei_roblib_orcid_oauth_auth_endpoint', 'https://sandbox.orcid.org/oauth/authorize'),
    '#description' => t('A valid URL for initial authorization.'),
  );

  $form['upei_roblib_orcid_oauth_client_id'] = array(
    '#title' => t('Client ID'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => variable_get('upei_roblib_orcid_oauth_client_id', ''),
    '#description' => t("This sites's ID for authenticating to the token server."),
  );

  $form['upei_roblib_orcid_oauth_client_secret'] = array(
    '#title' => t('Client secret'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => variable_get('upei_roblib_orcid_oauth_client_secret', ''),
    '#description' => t("This site's secret used for authenticating to the token server."),
  );

  $form['upei_roblib_orcid_mads_user_field'] = array(
    '#title' => t('Solr field to query for drupal user name'),
    '#type' => 'textfield',
    '#default_value' => variable_get('upei_roblib_orcid_mads_user_field', 'MADS_u1_ms'),
    '#description' => t('To match a drupal user with a Scholar profile we search for a drupal username
    in the MADS datastream. Enter the solr field where this is stored here. The default value is MADS_u1_ms'),
  );

  $form['upei_roblib_orcid_start_message'] = array(
    '#title' => t('Text to display with ORCID button.'),
    '#type' => 'textarea',
    '#default_value' => variable_get('upei_roblib_orcid_start_message', 'Connect your ORCID iD to
     IslandScholar. ORCID is a globally unique digital identifier that helps you link and identify
      your scholarly contributions. You control your ORCID, but it can be integrated with systems
       like IslandScholar, Google Scholar, ResearchGate, and others.'),
    '#description' => t('Text to display above the ORCID connect or create button.'),
  );

  $form['upei_roblib_orcid_logut_url'] = array(
    '#title' => t('The ORCID logut URL.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('upei_roblib_orcid_logut_url', 'https://orcid.org/userStatus.json?logUserOut=true'),
    '#description' => t('The URL to use when logging a user out of ORCID. Usually "https://orcid.org/userStatus.json?logUserOut=true".'),
  );

  $form['#validate'][] = 'upei_roblib_orcid_admin_form_validate';
  return system_settings_form($form);
}

/**
 * Validation handler for oauth2_authentication_admin_form.
 */
function upei_roblib_orcid_admin_form_validate($form, &$form_state) {
  $token_endpoint = $form_state['values']['upei_roblib_orcid_oauth_token_endpoint'];
  $auth_endpoint = $form_state['values']['upei_roblib_orcid_oauth_auth_endpoint'];
  if (!valid_url($token_endpoint, TRUE)) {
    form_set_error('upei_roblib_orcid_oauth_token_endpoint', 'The OAuth Token endpoint is not a valid URL.');
  }
  if (!valid_url($auth_endpoint, TRUE)) {
    form_set_error('upei_roblib_orcid_oauth_auth_endpoint', 'The OAuth Authorization endpoint is not a valid URL.');
  }
}
