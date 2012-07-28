<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xhtml="http://www.w3.org/1999/xhtml">

<xsl:output method='xml' encoding='utf-8'
	    doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
	    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" />

<xsl:template match="/portmap">
  <html>
    <head>
      <title>Running applications</title>
      <link href="/portmap/portmap.css" rel="stylesheet" type="text/css" />
    </head>
    <body>
      <h1>Running applications</h1>
      <table>
	<thead>
	  <tr>
	    <th>Service</th>
	    <th>User</th>
	    <th>Process ID</th>
	    <th>Port</th>
	    <th>Program</th>
	  </tr>
	</thead>
	<tbody>
	  <xsl:apply-templates select="instance">
	    <xsl:sort select="service" />
	    <xsl:sort select="user" />
	    <xsl:sort select="pid" />
	  </xsl:apply-templates>
	</tbody>
      </table>
    </body>
  </html>
</xsl:template>

<xsl:template match="/portmap/instance">
  <xsl:element name="tr">
    <xsl:attribute name="class">
      <xsl:choose>
	<xsl:when test="(position() mod 2) = 0">
	  <xsl:text>even</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>odd</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <xsl:element name="td">
      <xsl:value-of select="service" />
    </xsl:element>
    <xsl:element name="td">
      <xsl:value-of select="user" />
    </xsl:element>
    <xsl:element name="td">
      <xsl:value-of select="pid" />
    </xsl:element>
    <xsl:element name="td">
      <xsl:value-of select="port" />
    </xsl:element>
    <xsl:element name="td">
      <xsl:value-of select="prog" />
    </xsl:element>
  </xsl:element>
</xsl:template>

<!--

<xsl:template match="@*|node()">
  <xsl:copy>
    <xsl:apply-templates select="@*|node()"/>
  </xsl:copy>
</xsl:template>

 -->

</xsl:stylesheet>
