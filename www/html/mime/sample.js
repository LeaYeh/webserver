/**
 * This is a JavaScript file example
 * MIME Type: application/javascript
 */

// Function to display success message
function showMimeTypeSuccess() {
  console.log('MIME Type test successful!');
  
  // Create an element to display the success message
  const successElement = document.createElement('div');
  successElement.className = 'success-message';
  successElement.textContent = 'Successfully loaded JavaScript file! MIME Type: application/javascript';
  
  // Add the element to the page
  document.body.appendChild(successElement);
  
  // Display MIME type information
  const mimeInfo = {
    fileName: 'sample.js',
    mimeType: 'application/javascript',
    server: '42Vienna Web Server',
    testPurpose: 'Testing JavaScript file MIME type support'
  };
  
  console.table(mimeInfo);
}

// If in a browser environment, execute on page load
if (typeof window !== 'undefined') {
  window.addEventListener('load', showMimeTypeSuccess);
}

// Export function for use in Node.js environment
if (typeof module !== 'undefined' && module.exports) {
  module.exports = { showMimeTypeSuccess };
} 