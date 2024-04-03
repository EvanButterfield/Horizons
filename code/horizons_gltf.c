typedef struct gltf_buffer_view
{
  u32 Length;
  u32 Offset;
} gltf_buffer_view;

typedef struct gltf_accessor
{
  s32 BufferView;
  s32 Count;
} gltf_accessor;

internal s32
LoadGLTF(s8 *NameStr, game_mesh **Meshes_)
{
  string8 Name = CreateString(NameStr, &State->TempArena, Platform);
  platform_file_handle Handle = Platform->OpenFile(Name, true, FILE_OPEN_READ);
  u32 Size = Platform->GetFileSize(Handle);
  s8 *Data = PushSize(&State->TempArena, Size);
  Platform->ReadEntireFile(Handle, Size, Data);
  Platform->CloseFile(Handle);
  
  struct json_value_s *Root = json_parse(Data, Size);
  struct json_object_s *Object = (struct json_object_s *)Root->payload;
  
  struct json_object_element_s *AssetElement = Object->start;
  struct json_object_element_s *SearchElement = AssetElement->next;
  
  // Load binary buffer
  // NOTE(evan): Assume one binary file
  u8 *BinData;
  s32 BinSize;
  {
    struct json_array_s *BuffersArray = json_value_as_array(JSONFindValue(SearchElement, String8Plain("buffers")));
    struct json_object_element_s *BufferObjectElement = json_value_as_object(BuffersArray->start->value)->start;
    struct json_string_s *URIString = json_value_as_string(JSONFindValue(BufferObjectElement, String8Plain("uri")));
    
    platform_file_handle BinHandle = Platform->OpenFile(String8(URIString->string, (s32)URIString->string_size), true, FILE_OPEN_READ);
    BinSize = Platform->GetFileSize(BinHandle);
    BinData = PushSize(&State->TempArena, BinSize);
    Platform->ReadEntireFile(BinHandle, BinSize, BinData);
    Platform->CloseFile(BinHandle);
  }
  
  // Create buffer views
  gltf_buffer_view *BufferViews;
  s32 NumBufferViews;
  {
    struct json_array_s *BufferViewsArray = json_value_as_array(JSONFindValue(SearchElement, String8Plain("bufferViews")));
    if(BufferViewsArray == 0)
    {
      Platform->LogMessagePlain("Incorrect json format for glTF file\n", true, MESSAGE_SEVERITY_ERROR);
      return(0);
    }
    
    NumBufferViews = (s32)BufferViewsArray->length;
    BufferViews = PushArray(&State->TempArena, gltf_buffer_view, NumBufferViews);
    
    struct json_array_element_s *BufferViewElement = BufferViewsArray->start;
    for(s32 BufferViewIndex = 0; BufferViewIndex < NumBufferViews; ++BufferViewIndex)
    {
      struct json_object_element_s *BufferViewObjectElement = json_value_as_object(BufferViewElement->value)->start;
      
      struct json_value_s *LengthValue = JSONFindValue(BufferViewObjectElement, String8Plain("byteLength"));
      s32 Length = JSONGetS32FromValue(LengthValue);
      
      struct json_value_s *OffsetValue = JSONFindValue(BufferViewObjectElement, String8Plain("byteOffset"));
      s32 Offset = JSONGetS32FromValue(OffsetValue);
      
      gltf_buffer_view BufferView;
      BufferView.Length = Length;
      BufferView.Offset = Offset;
      BufferViews[BufferViewIndex] = BufferView;
      
      BufferViewElement = BufferViewElement->next;
    }
  }
  
  // Create accessors
  gltf_accessor *Accessors;
  s32 NumAccessors;
  {
    struct json_array_s *AccessorsArray = json_value_as_array(JSONFindValue(SearchElement, String8Plain("accessors")));
    if(AccessorsArray == 0)
    {
      Platform->LogMessagePlain("Incorrect json format for glTF file\n", true, MESSAGE_SEVERITY_ERROR);
      return(0);
    }
    
    NumAccessors = (s32)AccessorsArray->length;
    Accessors = PushArray(&State->TempArena, gltf_accessor, NumAccessors);
    
    struct json_array_element_s *AccessorElement = AccessorsArray->start;
    for(s32 AccessorIndex = 0; AccessorIndex < NumAccessors; ++AccessorIndex)
    {
      struct json_object_element_s *AccessorObjectElement = json_value_as_object(AccessorElement->value)->start;
      
      struct json_value_s *BufferViewValue = JSONFindValue(AccessorObjectElement, String8Plain("bufferView"));
      s32 BufferView = JSONGetS32FromValue(BufferViewValue);
      
      struct json_value_s *CountValue = JSONFindValue(AccessorObjectElement, String8Plain("count"));
      s32 Count = JSONGetS32FromValue(CountValue);
      
      gltf_accessor Accessor;
      Accessor.BufferView = BufferView;
      Accessor.Count = Count;
      
      Accessors[AccessorIndex] = Accessor;
      
      AccessorElement = AccessorElement->next;
    }
  }
  
  // Create materials
  game_material *Materials;
  s32 NumMaterials;
  {
    struct json_array_s *MaterialsArray = json_value_as_array(JSONFindValue(SearchElement, String8Plain("materials")));
    if(MaterialsArray == 0)
    {
      Platform->LogMessagePlain("Incorrect json format for glTF file\n", true, MESSAGE_SEVERITY_ERROR);
      return(0);
    }
    
    NumMaterials = (s32)MaterialsArray->length;
    Materials = PushArray(&State->PermArena, game_material, NumMaterials);
    
    struct json_array_element_s *MaterialElement = MaterialsArray->start;
    for(s32 MaterialIndex = 0; MaterialIndex < NumMaterials; ++MaterialIndex)
    {
      struct json_object_element_s *MaterialObjectElement = json_value_as_object(MaterialElement->value)->start;
      struct json_object_element_s *MRObjectElement = json_value_as_object(JSONFindValue(MaterialObjectElement, String8Plain("pbrMetallicRoughness")))->start;
      struct json_array_s *ColorArray = json_value_as_array(JSONFindValue(MRObjectElement, String8Plain("baseColorFactor")));
      if(ColorArray->length != 4)
      {
        Platform->LogMessagePlain("Incorrect json format for glTF file", true, MESSAGE_SEVERITY_ERROR);
        return(0);
      }
      
      struct json_array_element_s *ColorElement = ColorArray->start;
      
      vec4 Color;
      for(s32 ColorIndex = 0; ColorIndex < 4; ++ColorIndex)
      {
        Color.Elements[ColorIndex] = JSONGetFloatFromValue(ColorElement->value);
        ColorElement = ColorElement->next;
      }
      
      struct json_value_s *MetallicValue = JSONFindValue(MRObjectElement, String8Plain("metallicFactor"));
      f32 Metallic = JSONGetFloatFromValue(MetallicValue);
      
      struct json_value_s *RoughnessValue = JSONFindValue(MRObjectElement, String8Plain("roughnessFactor"));
      f32 Roughness = JSONGetFloatFromValue(RoughnessValue);
      
      game_material Material;
      Material.Color = Color;
      Material.Metallic = Metallic;
      Material.Roughness = Roughness;
      Materials[MaterialIndex] = Material;
      
      MaterialElement = MaterialElement->next;
    }
  }
  
  // Create meshes
  game_mesh *Meshes;
  s32 NumMeshes;
  {
    struct json_array_s *MeshesArray = json_value_as_array(JSONFindValue(SearchElement, String8Plain("meshes")));
    if(MeshesArray == 0)
    {
      Platform->LogMessagePlain("Incorrect json format for glTF file\n", true, MESSAGE_SEVERITY_ERROR);
      return(0);
    }
    
    NumMeshes = (s32)MeshesArray->length;
    Meshes = PushArray(&State->PermArena, game_mesh, NumMeshes);
    
    struct json_array_element_s *MeshElement = MeshesArray->start;
    for(s32 MeshIndex = 0; MeshIndex < NumMeshes; ++MeshIndex)
    {
      struct json_object_element_s *MeshObjectElement = json_value_as_object(MeshElement->value)->start;
      struct json_array_s *PrimitivesArray = json_value_as_array(JSONFindValue(MeshObjectElement, String8Plain("primitives")));
      
      // Assume one primitive
      struct json_object_element_s *PrimitiveObjectElement = json_value_as_object(PrimitivesArray->start->value)->start;
      
      struct json_object_element_s *AttributesObjectElement = json_value_as_object(JSONFindValue(PrimitiveObjectElement, String8Plain("attributes")))->start;
      
      struct json_value_s *PositionValue = JSONFindValue(AttributesObjectElement, String8Plain("POSITION"));
      s32 PositionAccessorIndex = JSONGetS32FromValue(PositionValue);
      gltf_accessor PositionAccessor = Accessors[PositionAccessorIndex];
      gltf_buffer_view PositionBufferView = BufferViews[PositionAccessor.BufferView];
      f32 *Positions_ = (f32 *)(BinData + PositionBufferView.Offset);
      s32 PositionCount = PositionAccessor.Count*3;
      f32 *Positions = PushArray(&State->TempArena, f32, PositionCount);
      Platform->CopyMemory(Positions, Positions_, sizeof(f32)*PositionCount);
      
      struct json_value_s *NormalValue = JSONFindValue(AttributesObjectElement, String8Plain("NORMAL"));
      s32 NormalAccessorIndex = JSONGetS32FromValue(NormalValue);
      gltf_accessor NormalAccessor = Accessors[NormalAccessorIndex];
      gltf_buffer_view NormalBufferView = BufferViews[NormalAccessor.BufferView];
      f32 *Normals_ = (f32 *)(BinData + NormalBufferView.Offset);
      s32 NormalCount = NormalAccessor.Count*3;
      f32 *Normals = PushArray(&State->TempArena, f32, NormalCount);
      Platform->CopyMemory(Normals, Normals_, sizeof(f32)*NormalCount);
      
      struct json_value_s *Texcoord0Value = JSONFindValue(AttributesObjectElement, String8Plain("TEXCOORD_0"));
      s32 Texcoord0AccessorIndex = JSONGetS32FromValue(Texcoord0Value);
      gltf_accessor Texcoord0Accessor = Accessors[Texcoord0AccessorIndex];
      gltf_buffer_view Texcoord0BufferView = BufferViews[Texcoord0Accessor.BufferView];
      f32 *UVs_ = (f32 *)(BinData + Texcoord0BufferView.Offset);
      s32 UVCount = Texcoord0Accessor.Count*2;
      f32 *UVs = PushArray(&State->TempArena, f32, UVCount);
      Platform->CopyMemory(UVs, UVs_, sizeof(f32)*UVCount);
      
      struct json_value_s *IndicesValue = JSONFindValue(PrimitiveObjectElement, String8Plain("indices"));
      s32 IndicesAccessorIndex = JSONGetS32FromValue(IndicesValue);
      gltf_accessor IndicesAccessor = Accessors[IndicesAccessorIndex];
      gltf_buffer_view IndicesBufferView = BufferViews[IndicesAccessor.BufferView];
      u16 *Indices16 = (u16 *)(BinData + IndicesBufferView.Offset);
      s32 IndexCount = IndicesAccessor.Count;
      u32 *Indices = PushArray(&State->PermArena, u32, IndexCount);
      for(s32 IndiceIndex = 0; IndiceIndex < IndexCount; ++IndiceIndex)
      {
        Indices[IndiceIndex] = (u32)Indices16[IndiceIndex];
      }
      
      s32 VertexCount = PositionCount/3;
      vertex *Vertices = PushArray(&State->PermArena, vertex, VertexCount);
      
      // NOTE: glTF stores the same position for 3 vertices for normal accuracy
      s32 CollisionPositionCount = VertexCount/3;
      vec3 *CollisionPositions = PushArray(&State->PermArena, vec3, CollisionPositionCount);
      
      s32 PositionsScanIndex = 0;
      s32 NormalsScanIndex = 0;
      s32 UVsScanIndex = 0;
      s32 CollisionPositionIndex = 0;
      for(s32 VertexIndex = 0; VertexIndex < VertexCount; ++VertexIndex)
      {
        vertex Vertex;
        Platform->CopyMemory(Vertex.Position.Elements, Positions + PositionsScanIndex, sizeof(vec3));
        Platform->CopyMemory(Vertex.Normal.Elements, Normals + NormalsScanIndex, sizeof(vec3));
        Platform->CopyMemory(Vertex.UV.Elements, UVs + UVsScanIndex, sizeof(vec2));
        Vertex.Color = Vec3(1, 1, 1);
        
        Vertices[VertexIndex] = Vertex;
        
        if((VertexIndex + 1) % 3 == 0)
        {
          Platform->CopyMemory(&CollisionPositions[CollisionPositionIndex], Vertex.Position.Elements, sizeof(vec3));
          ++CollisionPositionIndex;
        }
        
        PositionsScanIndex += 3;
        NormalsScanIndex += 3;
        UVsScanIndex += 2;
      }
      
      platform_mesh PlatformMesh = Platform->CreateMesh(Vertices, VertexCount, Indices, IndexCount);
      
      struct json_value_s *MaterialValue = JSONFindValue(PrimitiveObjectElement, String8Plain("material"));
      s32 MaterialIndex = JSONGetS32FromValue(MaterialValue);
      game_material *Material = &Materials[MaterialIndex];
      
      game_mesh Mesh;
      Mesh.Mesh = PlatformMesh;
      Mesh.CollisionPositions = CollisionPositions;
      Mesh.CollisionPositionCount = CollisionPositionCount;
      Mesh.Material = Material;
      Meshes[MeshIndex] = Mesh;
      
      PopArray(&State->TempArena, f32, PositionCount + UVCount);
      MeshElement = MeshElement->next;
    }
  }
  
  PopArray(&State->TempArena, gltf_accessor, NumAccessors);
  PopArray(&State->TempArena, gltf_buffer_view, NumBufferViews);
  PopSize(&State->TempArena, BinSize);
  PopSize(&State->TempArena, Size);
  free(Root);
  
  *Meshes_ = Meshes;
  return(NumMeshes);
}