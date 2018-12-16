
class NIFXML
  attr_reader :version, :versions, :types

  def initialize
    @version = nil
    @versions = []
    @types = {}
  end

  def self.parse(filename)
    desc = self.new

    doc =
      File.open(filename, 'r') do |io|
        REXML::Document.new io
      end

    desc.load_xml doc.root

    desc
  end

  def load_xml(document)
    @version = document['version']

    document.each_child do |child|
      if child.kind_of? REXML::Element
        case child.name
        when "version"
          parse_version child

        when "basic"
          parse_basic child

        when "bitflags"
          parse_bitflags child

        when "enum"
          parse_enum child

        when "compound"
          parse_compound child

        when "niobject"
          parse_niobject child

        else
          raise "unsupported element at root: #{child.name}"
        end
      end
    end
  end

  private

  def parse_version(element)
    @versions.push NIFVersion.new element['num'], element.text
  end

  def parse_basic(element)
    basic = NIFBasic.new
    parse_type element, basic
    register_type basic
  end

  def parse_type(element, type)
    type.name = element['name']
    if element['count']
      type.count = Integer(element['count'], 10)
    end
    if element['istemplate']
      type.template = Integer(element['istemplate']) != 0
    end
    type.description = element.text
    unless type.description.nil?
      type.description = type.description.strip
    end
    type
  end

  def parse_bitflags(element)
    bitflags = NIFBitflags.new
    parse_type element, bitflags
    parse_enum_options element, bitflags
    register_type bitflags
  end

  def parse_enum(element)
    enum = NIFEnum.new
    parse_type element, enum
    parse_enum_options element, enum
    register_type enum
  end

  def parse_enum_options(element, type)
    type.storage = element["storage"]
    type.prefix = element["prefix"]

    element.each_child do |child|
      if child.kind_of? REXML::Element
        case child.name
        when "option"
          option = NIFOption.new
          option.name = child["name"]
          option.value = Integer(child["value"], 0)
          option.description = child.text
          type.options.push option
        else
          raise "unknown child in enum options: #{child.name}"
        end
      end
    end
  end

  def parse_compound(element)
    compound = NIFCompound.new
    parse_type element, compound
    parse_compound_content element, compound
    register_type compound
  end

  def parse_niobject(element)
    compound = NIFNiObject.new
    parse_type element, compound
    parse_compound_content element, compound
    register_type compound
  end

  def parse_compound_content(element, compound)
    compound.ver1 = element['ver1']
    compound.ver2 = element['ver2']

    if element['abstract']
      compound.abstract = Integer(element['abstract']) != 0
    end
    if element['inherit']
      compound.inherits = element['inherit']
    end

    element.each_child do |child|
      if child.kind_of? REXML::Element
        case child.name
        when "add"
          field = NIFField.new
          field.name = child['name']
          field.type = child['type']
          field.ver1 = child['ver1']
          field.ver2 = child['ver2']
          field.arr1 = child['arr1']
          field.arr2 = child['arr2']

          if child['template']
            field.template_type = child['template']
          end

          field.default = child['default']
          field.vercond = child['vercond']
          field.cond = child['cond']
          field.arg = child['arg']
          if child['calculated']
            field.calculated = Integer(child['calculated'], 0) != 0
          end
          field.userver = child['userver']
          field.userver2 = child['userver2']
          field.suffix = child['suffix']
          if child['binary']
            field.binary = Integer(child['binary'], 0) != 0
          end
          if child['abstract']
            field.abstract = Integer(child['abstract'], 0) != 0
          end
          field.description = child.text
          compound.fields.push field
        else
          raise "unknown child in compound content: #{child.name}"
        end
      end
    end
  end

  def register_type(type)
    if @types.include? type.name
      raise "type #{type.name} redefined"
    end
    @types[type.name] = type
    type
  end
end
