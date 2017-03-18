<?php

/**
 * @file
 * Provides the administration page for Equal Heights.
 */

/**
 * Administration settings page.
 */

function jquery_matchheight_admin($form, $form_state) {
  $form = array();

  $form['jquery_matchheight_classes'] = array(
    '#tree' => TRUE,
    '#theme' => 'jquery_matchheight_classes',
  );

  // See how many rows there should be.
  $classes = variable_get("jquery_matchheight_css_classes", array());
  $rows_count = 0;
  if (isset($form_state['rows_count'])) {
    $rows_count = $form_state['rows_count'];
  }
  else {
    $rows_count = count($classes) + 1;
  }
  for ($index = 0; $index < $rows_count; $index++) {
    if (isset($classes[$index])) {
      $setting = $classes[$index];
      $form['jquery_matchheight_classes'][$index] = jquery_matchheight_settings_fields($setting);
    }
    else {
      $form['jquery_matchheight_classes'][$index] = jquery_matchheight_settings_fields();
    }
  }
  $form['more'] = array(
    '#type' => 'submit',
    '#value' => t('Add row'),
    '#description' => t('Add a new row'),
    '#submit' => array('jquery_matchheight_more_submit'),
    '#ajax' => array(
      'callback' => 'jquery_matchheight_ajax',
      'wrapper' => 'jquery_matchheight_wrapper',
      'method' => 'replace',
      'effect' => 'none',
    ),
  );

  $form['imagesloaded'] = array(
    '#type' => 'checkbox',
    '#description' => t('Load the minified version of the imagesloaded plugin for performance.'),
    '#default_value' => variable_get('jquery_matchheight_imagesloaded_min', TRUE),
    '#title' => t('Enable midisablenified imagesloaded plugin'),
  );
  $form['imagesloaded_ie8'] = array(
    '#type' => 'checkbox',
    '#description' => t('Disable imagesloaded plugin for <=Internet Explorer 8 (has been known to cause issues).'),
    '#default_value' => variable_get('jquery_matchheight_imagesloaded_iele8', TRUE),
    '#title' => t('Disable imagesloaded plugin for IE8 and earlier (recommended)'),
  );
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),

  );

  return $form;
}
function jquery_matchheight_admin_validate($form, &$form_state) {

  foreach ($form_state['values']['jquery_matchheight_classes'] as $i => $class) {
    if (!empty($class['selector'])) {
      if (!empty($class['minheight']) && !is_numeric($class['minheight'])) {
        form_set_error('', t('Min height must be a number.'));
      }
      elseif (!empty($class['maxheight']) && !is_numeric($class['maxheight'])) {
        form_set_error('', t('Max height must be a number.'));
      }
    }
  }
}

function jquery_matchheight_admin_submit($form, &$form_state) {
  $settings = array();

  foreach ($form_state['values']['jquery_matchheight_classes'] as $i => $class) {
    if (!empty($class['selector']) && $class['delete'] == 0) {
      // If only maxheight is set, make minheight equal to '0'
      if (!$class['minheight'] && !empty($class['maxheight'])) {
        $class['minheight'] = '0';
      }
      $settings[] = array(
        'selector' => _jquery_matchheight_selector_fold($class['selector']),
        'mediaquery' => trim($class['mediaquery']),
        'minheight' => trim($class['minheight']),
        'maxheight' => trim($class['maxheight']),
        'overflow' => $class['overflow'],
      );
    }
  }
  variable_set('jquery_matchheight_css_classes', $settings);
  variable_set('jquery_matchheight_imagesloaded_min', $form_state['values']['imagesloaded']);
  variable_set('jquery_matchheight_imagesloaded_iele8', $form_state['values']['imagesloaded_ie8']);
  backdrop_set_message(t('Settings have been set.'));
}

/**
 * Remove newlines from selector.
 *
 * New lines are not allowed in the jQuery selector. Queries like:
 *   jQuery(".region\n .header");
 * fail.
 */
function _jquery_matchheight_selector_fold($selector) {
  return trim(trim(preg_replace(array("/\s+/", "/\n/", "/\r/"), ' ', $selector)), ',');
}

/**
 * Make a better user readable version of the selector.
 *
 * Replace comma by comma followed by a new line.
 * Spaces before and after a comma are removed.
 */
function _jquery_matchheight_selector_unfold($selector) {
  $select = _jquery_matchheight_selector_fold($selector);
  return preg_replace("/\s*\,\s*/", ",\n", $selector);
}

function jquery_matchheight_settings_fields($settings = array()) {
  // Add defaults
  $settings += array(
      'selector' => '',
      'minheight' => '',
      'maxheight' => '',
      'overflow' => 'auto',
      'mediaquery' => '',
    );
  $selector = $settings['selector'];
  $minheight = $settings['minheight'];
  $maxheight = $settings['maxheight'];
  $overflow = $settings['overflow'];
  $mediaquery = $settings['mediaquery'];

  $form['#tree'] = TRUE;
  $form['delete'] = array(
    '#type' => 'checkbox',
    '#default' => FALSE,
    '#attributes' => array(
      'title' => t('Delete the class'),
    ),
  );
  $form['selector'] = array(
    '#type' => 'textarea',
    '#default_value' => _jquery_matchheight_selector_unfold($selector),
    '#rows' => 3,
    '#cols' => 20,
    '#attributes' => array(
      'title' => t("The selector of the elements that need equal height"),
    ),
    '#description' => t("You may enter one or several compound selectors seperated by a comma.<br>
                         New lines will be injected before commas."),
  );
  $form['mediaquery'] = array(
    '#type' => 'textarea',
    '#default_value' => $mediaquery,
    '#rows' => 3,
    '#cols' => 20,
    '#attributes' => array(
        'title' => t("The media query to use for this line"),
      ),
    '#description' => t('You may enter only one media query like "(min-width: 480px)".<br>
                     Equalheights will be loaded for matching mediaquery'),
  );
  $form['minheight'] = array(
    '#type' => 'textfield',
    '#default_value' => $minheight,
    '#size' => 4,
    '#attributes' => array(
      'title' => t('Minimum height of the class, in pixels'),
    ),
  );
  $form['maxheight'] = array(
    '#type' => 'textfield',
    '#default_value' => $maxheight,
    '#size' => 4,
    '#attributes' => array(
      'title' => t('Maximum height of the class, in pixels'),
    ),
  );
  $form['overflow'] = array(
    '#type' => 'radios',
    '#default_value' => $overflow,
    '#options' => array(
      'auto' => t('auto'),
      'hidden' => t('hidden'),
      'visible' => t('visible'),
    ),
    '#attributes' => array(
      'title' => t('Overflow value if the content taller than the max height'),
    ),
  );
  return $form;
}

function theme_jquery_matchheight_classes($variables) {
  $form = $variables['form'];
  $rows = array();
  foreach (element_children($form) as $key) {
    // Build the table row.
    $rows[$key] = array(
      'data' => array(
        array('data' => backdrop_render($form[$key]['selector']), 'class' => 'matchheight-class'),
        array('data' => backdrop_render($form[$key]['mediaquery']), 'class' => 'matchheight-class'),
        array('data' => backdrop_render($form[$key]['minheight']), 'class' => 'matchheight-minheight'),
        array('data' => backdrop_render($form[$key]['maxheight']), 'class' => 'matchheight-mzxheight'),
        array('data' => backdrop_render($form[$key]['overflow']), 'class' => 'matchheight-overflow'),
        array('data' => backdrop_render($form[$key]['delete']), 'class' => 'matchheight-delete'),
      ),
    );
    // Add any attributes on the element to the row, such as the ahah class.
    if (array_key_exists('#attributes', $form[$key])) {
      $rows[$key] = array_merge($rows[$key], $form[$key]['#attributes']);
     }
 }
  $header = array(
    array('data' => t('Class or tag of the element'), 'title' => t('The class of the element to be used with jQuery matchHeight.')),
    array('data' => t('Media query to filter on'), 'title' => t('The media query.')),
    array('data' => t('Min height (optional)'), 'title' => t('Minimum height of the element')),
    array('data' => t('Max height (optional)'), 'title' => t('Maximum height of the element')),
    array('data' => t('Overflow'), 'title' => t('Overflow value')),
    array('data' => t('Delete'), 'title' => t('Delete class.')),
  );

  $output = theme('table', array(
   'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'jquery_matchheight_wrapper'),
  ));
  $output .= backdrop_render_children($form);
  return $output;
}

function jquery_matchheight_ajax($form, &$form_state) {
  return $form['jquery_matchheight_classes'];
}

function jquery_matchheight_more_submit($form, &$form_state) {
  $form_state['rows_count'] = count($form_state['values']['jquery_matchheight_classes']) + 1;
  $form_state['rebuild'] = TRUE;
}
