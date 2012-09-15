<?xml version='1.0'?>
<xsl:stylesheet  
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:template name="body.attributes">
  <xsl:choose>
    <xsl:when test='@id'>
      <xsl:attribute name="id"><xsl:text>body</xsl:text><xsl:value-of select='@id' /></xsl:attribute>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:include href="http://docbook.sourceforge.net/release/xsl/current/xhtml-1_1/chunk.xsl" />

</xsl:stylesheet>
