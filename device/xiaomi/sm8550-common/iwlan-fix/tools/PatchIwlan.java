import java.io.*;
import java.nio.file.*;
import java.security.*;
import java.util.*;
import org.jf.dexlib2.*;
import org.jf.dexlib2.builder.*;
import org.jf.dexlib2.builder.instruction.*;
import org.jf.dexlib2.dexbacked.*;
import org.jf.dexlib2.iface.*;
import org.jf.dexlib2.iface.instruction.*;
import org.jf.dexlib2.iface.reference.*;
import org.jf.dexlib2.immutable.*;
import org.jf.dexlib2.immutable.reference.*;
import org.jf.dexlib2.util.*;
import org.jf.dexlib2.writer.pool.DexPool;

/** Hash-gated by build.py; all non-whitelisted original methods are semantically inventoried. */
public final class PatchIwlan {
    static final String P="Lvendor/qti/iwlan/IWlanProxy;", R="Lvendor/qti/iwlan/IWlanRequest;";
    static final String H="Lvendor/qti/iwlan/IWlanWakeLockFix;", S="Lvendor/qti/iwlan/IWlanWakeLockFix$State;";
    static final String RESP="Lvendor/qti/iwlan/IWlanResponse;", HANDLER="Lvendor/qti/iwlan/IWlanProxy$1;";
    static final String HAL="Lvendor/qti/hardware/data/iwlan/V1_1/IIWlan;", INFO="Lvendor/qti/hardware/data/iwlan/V1_0/IWlanResponseInfo;";
    static final String MSG="Landroid/os/Message;", STR="Ljava/lang/String;", OBJ="Ljava/lang/Object;";
    static final Map<String,String> DELEGATES=new LinkedHashMap<>();
    static final Map<String,String> SENDS=new LinkedHashMap<>();
    static {
        DELEGATES.put("obtainRequest","obtain");DELEGATES.put("acquireWakeLock","acquire");
        DELEGATES.put("decrementWakeLock","release");DELEGATES.put("clearWakeLock","clear");
        DELEGATES.put("findAndRemoveRequestFromList","take");DELEGATES.put("processRequestAck","earlyAck");
        DELEGATES.put("processResponse","processResponse");
        SENDS.put("deactivateDataCall","sendDeactivate");SENDS.put("requestDataCallList","sendDataCallList");
        SENDS.put("getDataRegistrationState","sendRegistration");SENDS.put("getAllQualifiedNetworks","sendQualified");
        SENDS.put("setupDataCall","sendSetup");SENDS.put("setupDataCall_1_1","sendSetup11");
    }
    static ImmutableMethodReference ref(String c,String n,List<String> p,String r){return new ImmutableMethodReference(c,n,p,r);}
    static List<String> params(MethodReference m){List<String> p=new ArrayList<>();for(CharSequence t:m.getParameterTypes())p.add(t.toString());return p;}
    static int words(String t){return t.equals("J")||t.equals("D")?2:1;}
    static int insWords(Method m){int n=AccessFlags.STATIC.isSet(m.getAccessFlags())?0:1;for(String t:params(m))n+=words(t);return n;}
    static ImmutableMethod method(Method m,String name,int flags,MethodImplementation impl){return new ImmutableMethod(m.getDefiningClass(),name,m.getParameters(),m.getReturnType(),flags,m.getAnnotations(),m.getHiddenApiRestrictions(),impl);}
    static MethodImplementation impl(int regs,List<? extends Instruction> body){return new ImmutableMethodImplementation(regs,body,Collections.emptyList(),Collections.emptyList());}
    static BuilderInstruction35c invoke(Opcode op,MethodReference target,int... regs){int[] r=new int[5];System.arraycopy(regs,0,r,0,regs.length);return new BuilderInstruction35c(op,regs.length,r[0],r[1],r[2],r[3],r[4],target);}
    static Method delegate(Method m,String helper){
        List<String> ps=params(m);ps.add(0,P);int n=insWords(m);List<Instruction> b=new ArrayList<>();
        b.add(new BuilderInstruction3rc(Opcode.INVOKE_STATIC_RANGE,1,n,ref(H,helper,ps,m.getReturnType())));
        if(m.getReturnType().equals("V"))b.add(new BuilderInstruction10x(Opcode.RETURN_VOID));
        else {boolean object=m.getReturnType().startsWith("L")||m.getReturnType().startsWith("[");b.add(new BuilderInstruction11x(object?Opcode.MOVE_RESULT_OBJECT:Opcode.MOVE_RESULT,0));b.add(new BuilderInstruction11x(object?Opcode.RETURN_OBJECT:Opcode.RETURN,0));}
        return method(m,m.getName(),m.getAccessFlags(),impl(n+1,b));
    }
    static Method reset(Method m){List<Instruction>b=new ArrayList<>();b.add(invoke(Opcode.INVOKE_STATIC,ref(H,"reset",Arrays.asList(P),"V"),0));b.add(invoke(Opcode.INVOKE_DIRECT,ref(P,"getService",Collections.emptyList(),HAL),0));b.add(new BuilderInstruction10x(Opcode.RETURN_VOID));return method(m,m.getName(),m.getAccessFlags(),impl(1,b));}
    static Method finalHelper(Method m){
        String name=m.getName();boolean isVoid=name.equals("responseVoid");boolean ack=isVoid||name.equals("responseSetupDataCall");
        String tag=name.equals("responseDataRegistrationState")?"Lvendor/qti/iwlan/IWlanNetworkService;":name.equals("responseQualifiedNetworksChanged")?"Lvendor/qti/iwlan/QualifiedNetworksServiceImpl;":"Lvendor/qti/iwlan/IWlanDataService;";
        List<Instruction>b=new ArrayList<>();b.add(new BuilderInstruction22c(Opcode.IGET_OBJECT,0,5,new ImmutableFieldReference(RESP,"mProxy",P)));
        b.add(new BuilderInstruction12x(Opcode.MOVE_OBJECT,1,6));
        b.add(isVoid?new BuilderInstruction11n(Opcode.CONST_4,2,0):new BuilderInstruction12x(Opcode.MOVE_OBJECT,2,7));
        b.add(new BuilderInstruction11n(Opcode.CONST_4,3,ack?1:0));
        b.add(isVoid?new BuilderInstruction12x(Opcode.MOVE_OBJECT,4,7):new BuilderInstruction21c(Opcode.SGET_OBJECT,4,new ImmutableFieldReference(tag,"TAG",STR)));
        b.add(new BuilderInstruction3rc(Opcode.INVOKE_STATIC_RANGE,0,5,ref(H,"complete",Arrays.asList(P,INFO,OBJ,"Z",STR),"V")));
        b.add(new BuilderInstruction10x(Opcode.RETURN_VOID));return method(m,name,m.getAccessFlags(),impl(8,b));
    }
    static Method handler(Method m){
        MutableMethodImplementation b=new MutableMethodImplementation(m.getImplementation());Label old=b.newLabelForIndex(0);
        int self=m.getImplementation().getRegisterCount()-2,msg=self+1;
        List<BuilderInstruction> prefix=Arrays.asList(new BuilderInstruction22c(Opcode.IGET_OBJECT,0,self,new ImmutableFieldReference(HANDLER,"this$0",P)),invoke(Opcode.INVOKE_STATIC,ref(H,"handleEvent",Arrays.asList(P,MSG),"Z"),0,msg),new BuilderInstruction11x(Opcode.MOVE_RESULT,0),new BuilderInstruction21t(Opcode.IF_EQZ,0,old),new BuilderInstruction10x(Opcode.RETURN_VOID));
        for(int i=0;i<prefix.size();i++)b.addInstruction(i,prefix.get(i));
        return method(m,m.getName(),m.getAccessFlags(),b);
    }
    static int[] invokeRegs(Instruction x){
        if(x instanceof RegisterRangeInstruction){RegisterRangeInstruction r=(RegisterRangeInstruction)x;int[]v=new int[r.getRegisterCount()];for(int i=0;i<v.length;i++)v[i]=r.getStartRegister()+i;return v;}
        FiveRegisterInstruction r=(FiveRegisterInstruction)x;return Arrays.copyOf(new int[]{r.getRegisterC(),r.getRegisterD(),r.getRegisterE(),r.getRegisterF(),r.getRegisterG()},r.getRegisterCount());
    }
    static BuilderInstruction move(String type,int to,int from){return new BuilderInstruction32x(type.startsWith("L")||type.startsWith("[")?Opcode.MOVE_OBJECT_16:words(type)==2?Opcode.MOVE_WIDE_16:Opcode.MOVE_16,to,from);}
    static Method send(Method m){
        MethodImplementation before=m.getImplementation();MutableMethodImplementation b=new MutableMethodImplementation(before);
        List<? extends BuilderInstruction> originals=new ArrayList<>(b.getInstructions());
        int total=before.getRegisterCount(),self=total-insWords(m),req=m.getName().equals("setupDataCall")?11:m.getName().equals("setupDataCall_1_1")?13:1;
        int extra=total,error=extra+3,pc=0,afterObtain=-1,halIndex=-1;String helper=SENDS.get(m.getName());
        Map<Integer,Integer> indices=new HashMap<>();Map<Integer,Label> labels=new HashMap<>();
        for(int i=0;i<originals.size();i++){indices.put(pc,i);labels.put(pc,b.newLabelForIndex(i));Instruction x=originals.get(i);
            if(x instanceof ReferenceInstruction&&((ReferenceInstruction)x).getReference() instanceof MethodReference){MethodReference mr=(MethodReference)((ReferenceInstruction)x).getReference();if(mr.getDefiningClass().equals(P)&&mr.getName().equals("obtainRequest"))afterObtain=i+2;if(mr.getDefiningClass().equals(HAL)&&mr.getName().equals(m.getName().equals("requestDataCallList")?"getDataCallList":m.getName())){if(halIndex!=-1)throw new IllegalStateException("multiple target HAL calls");halIndex=i;}}
            pc+=x.getCodeUnits();}
        labels.put(pc,b.newLabelForIndex(originals.size()));
        if(afterObtain<0||halIndex<0)throw new IllegalStateException("missing source anchor "+m);
        Map<Integer,List<BuilderInstruction>> inserts=new TreeMap<>(Collections.reverseOrder());
        Set<Integer> handlers=new HashSet<>();for(TryBlock<? extends ExceptionHandler> t:before.getTryBlocks())for(ExceptionHandler eh:t.getExceptionHandlers())handlers.add(eh.getHandlerCodeAddress());
        for(int address:handlers){int index=indices.get(address);Instruction x=originals.get(index);if(x.getOpcode()!=Opcode.MOVE_EXCEPTION)throw new IllegalStateException("handler anchor");int e=((OneRegisterInstruction)x).getRegisterA();inserts.computeIfAbsent(index+1,k->new ArrayList<>()).addAll(Arrays.asList(move(P,extra,self),move(R,extra+1,req),move("Ljava/lang/Throwable;",extra+2,e),new BuilderInstruction3rc(Opcode.INVOKE_STATIC_RANGE,extra,3,ref(H,"failRequest",Arrays.asList(P,R,"Ljava/lang/Throwable;"),"V"))));}
        for(Map.Entry<Integer,List<BuilderInstruction>> e:inserts.entrySet())for(int i=0;i<e.getValue().size();i++)b.addInstruction(e.getKey()+i,e.getValue().get(i));
        // Copy the newly shifted incoming parameters into their original registers. All original
        // instructions continue to see the same register values; the four extra registers are scratch.
        List<String> incoming=params(m);incoming.add(0,P);int old=self;List<BuilderInstruction> preamble=new ArrayList<>();
        for(String type:incoming){preamble.add(move(type,old,old+4));old+=words(type);}for(int i=0;i<preamble.size();i++)b.addInstruction(i,preamble.get(i));
        // Preparation/logging after obtain was outside the vendor's HAL try. Cover it explicitly.
        Label prep=labels.get(addressOf(originals,afterObtain));int firstTry=before.getTryBlocks().stream().mapToInt(TryBlock::getStartCodeAddress).min().getAsInt();Label prepEnd=labels.get(firstTry);
        Label terminal=b.newLabelForIndex(b.getInstructions().size());b.addInstruction(new BuilderInstruction11x(Opcode.MOVE_EXCEPTION,error));
        b.addInstruction(move(P,extra,self));b.addInstruction(move(R,extra+1,req));b.addInstruction(move("Ljava/lang/Throwable;",extra+2,error));
        b.addInstruction(new BuilderInstruction3rc(Opcode.INVOKE_STATIC_RANGE,extra,3,ref(H,"failRequest",Arrays.asList(P,R,"Ljava/lang/Throwable;"),"V")));
        b.addInstruction(new BuilderInstruction11x(Opcode.THROW,error));
        if(addressOf(originals,afterObtain)<firstTry)b.addCatch(prep,prepEnd,terminal);
        for(TryBlock<? extends ExceptionHandler> t:before.getTryBlocks())b.addCatch(labels.get(t.getStartCodeAddress()),labels.get(t.getStartCodeAddress()+t.getCodeUnitCount()),terminal);
        MethodImplementation result=new ImmutableMethodImplementation(total+4,b.getInstructions(),b.getTryBlocks(),b.getDebugItems());
        return method(m,m.getName(),m.getAccessFlags(),result);
    }
    static int addressOf(List<? extends Instruction> ins,int index){int p=0;for(int i=0;i<index;i++)p+=ins.get(i).getCodeUnits();return p;}
    static DexBackedDexFile read(String path)throws Exception{try(InputStream i=new BufferedInputStream(new FileInputStream(path))){return DexBackedDexFile.fromInputStream(Opcodes.forApi(35),i);}}
    static String key(Method m){return m.getDefiningClass()+"->"+m.getName()+params(m)+m.getReturnType();}
    static boolean allowed(Method m){return m.getDefiningClass().equals(P)&&(DELEGATES.containsKey(m.getName())||SENDS.containsKey(m.getName())||m.getName().equals("resetProxyAndRequestList"))||m.getDefiningClass().equals(RESP)&&new HashSet<>(Arrays.asList("responseSetupDataCall","responseVoid","responseDataCallList","responseDataRegistrationState","responseQualifiedNetworksChanged")).contains(m.getName())||m.getDefiningClass().equals(HANDLER)&&m.getName().equals("handleMessage");}

    /** Canonical symbolic operands remove only serialized ID/index/offset-table differences. */
    static String code(Method m){
        StringBuilder s=new StringBuilder();MethodImplementation b=m.getImplementation();if(b==null)return "abstract";s.append(b.getRegisterCount()).append('|');
        for(Instruction x:b.getInstructions()){
            s.append(x.getOpcode()).append(':');
            if(x instanceof FiveRegisterInstruction)s.append(Arrays.toString(invokeRegs(x)));
            else if(x instanceof RegisterRangeInstruction)s.append(Arrays.toString(invokeRegs(x)));
            else {if(x instanceof OneRegisterInstruction)s.append(((OneRegisterInstruction)x).getRegisterA()).append(',');if(x instanceof TwoRegisterInstruction)s.append(((TwoRegisterInstruction)x).getRegisterB()).append(',');if(x instanceof ThreeRegisterInstruction)s.append(((ThreeRegisterInstruction)x).getRegisterC()).append(',');}
            if(x instanceof WideLiteralInstruction)s.append(((WideLiteralInstruction)x).getWideLiteral());
            if(x instanceof ReferenceInstruction)s.append(ReferenceUtil.getReferenceString(((ReferenceInstruction)x).getReference()));
            if(x instanceof DualReferenceInstruction)s.append(ReferenceUtil.getReferenceString(((DualReferenceInstruction)x).getReference2()));
            if(x instanceof OffsetInstruction)s.append('@').append(((OffsetInstruction)x).getCodeOffset());
            if(x instanceof org.jf.dexlib2.iface.instruction.SwitchPayload)for(SwitchElement e:((org.jf.dexlib2.iface.instruction.SwitchPayload)x).getSwitchElements())s.append(e.getKey()).append('=').append(e.getOffset()).append(',');
            if(x instanceof org.jf.dexlib2.iface.instruction.formats.ArrayPayload)s.append(((org.jf.dexlib2.iface.instruction.formats.ArrayPayload)x).getElementWidth()).append(((org.jf.dexlib2.iface.instruction.formats.ArrayPayload)x).getArrayElements());
            s.append(';');
        }
        for(TryBlock<? extends ExceptionHandler> t:b.getTryBlocks()){s.append("TRY").append(t.getStartCodeAddress()).append('/').append(t.getCodeUnitCount());for(ExceptionHandler h:t.getExceptionHandlers())s.append(h.getExceptionType()).append('@').append(h.getHandlerCodeAddress());}
        return s.toString();
    }
    static String sha(String s)throws Exception{byte[]h=MessageDigest.getInstance("SHA-256").digest(s.getBytes("UTF-8"));StringBuilder b=new StringBuilder();for(byte x:h)b.append(String.format("%02x",x));return b.toString();}
    static void verify(DexFile old,DexFile now,Path report)throws Exception{
        Map<String,ClassDef>a=new TreeMap<>(),b=new TreeMap<>();for(ClassDef c:old.getClasses())a.put(c.getType(),c);for(ClassDef c:now.getClasses())b.put(c.getType(),c);
        if(!a.keySet().equals(b.keySet()))throw new IllegalStateException("original dex class inventory changed");
        int untouched=0,changed=0;List<String> ledger=new ArrayList<>();
        for(String t:a.keySet()){
            ClassDef x=a.get(t),y=b.get(t);if(x.getAccessFlags()!=y.getAccessFlags()||!Objects.equals(x.getSuperclass(),y.getSuperclass())||!x.getInterfaces().equals(y.getInterfaces())||!Objects.equals(x.getSourceFile(),y.getSourceFile())||!x.getAnnotations().equals(y.getAnnotations()))throw new IllegalStateException("class metadata "+t);
            Map<String,Field>xf=new TreeMap<>(),yf=new TreeMap<>();for(Field f:x.getFields())xf.put(f.getName(),f);for(Field f:y.getFields())yf.put(f.getName(),f);
            if(t.equals(P)){Field added=yf.remove("mLeaseState");if(added==null||!added.getType().equals(S)||added.getAccessFlags()!=AccessFlags.VOLATILE.getValue())throw new IllegalStateException("state field");}
            if(!xf.keySet().equals(yf.keySet()))throw new IllegalStateException("fields "+t);
            for(String f:xf.keySet()){Field f1=xf.get(f),f2=yf.get(f);boolean defaults=(f1.getInitialValue()==null||EncodedValueUtils.isDefaultValue(f1.getInitialValue()))&&(f2.getInitialValue()==null||EncodedValueUtils.isDefaultValue(f2.getInitialValue()));if(f1.getAccessFlags()!=f2.getAccessFlags()||!f1.getType().equals(f2.getType())||!f1.getAnnotations().equals(f2.getAnnotations())||(!defaults&&!Objects.equals(f1.getInitialValue(),f2.getInitialValue())))throw new IllegalStateException("field metadata "+f1);}
            Map<String,Method>ym=new TreeMap<>();for(Method m:y.getMethods())ym.put(key(m),m);
            for(Method m:x.getMethods()){
                Method n=ym.remove(key(m));if(n==null||m.getAccessFlags()!=n.getAccessFlags()||!m.getAnnotations().equals(n.getAnnotations())||!m.getParameters().equals(n.getParameters()))throw new IllegalStateException("method metadata "+m);
                String before=code(m),after=code(n);if(allowed(m)){if(before.equals(after))throw new IllegalStateException("white list method not patched "+m);changed++;ledger.add("{\"method\":\""+key(m)+"\",\"before\":\""+sha(before)+"\",\"after\":\""+sha(after)+"\"}");}
                else{if(!before.equals(after))throw new IllegalStateException("unrelated method changed "+m);untouched++;}
            }
            if(!ym.isEmpty())throw new IllegalStateException("unapproved extra method");
        }
        if(changed!=20)throw new IllegalStateException("expected20 patched methods, got"+changed);
        Files.write(report,("{\"originalClasses\":"+a.size()+",\"unchangedMethods\":"+untouched+",\"changedMethods\":"+changed+",\"addedProxyMethods\":0,\"addedProxyFields\":1,\"originalGetServiceUnchanged\":true,\"whitelistPassed\":true,\"methods\":["+String.join(",",ledger)+"]}\n").getBytes("UTF-8"));
    }
    public static void main(String[]args)throws Exception{
        DexBackedDexFile source=read(args[0]);List<ClassDef> classes=new ArrayList<>();
        if(args.length==4&&args[3].equals("--verify")){verify(source,read(args[1]),Paths.get(args[2]));return;}
        for(ClassDef c:source.getClasses()){
            List<Field> fields=new ArrayList<>();for(Field f:c.getFields())fields.add(f);List<Method> methods=new ArrayList<>();
            for(Method m:c.getMethods()){
                if(c.getType().equals(P)&&DELEGATES.containsKey(m.getName())){methods.add(delegate(m,DELEGATES.get(m.getName())));}
                else if(c.getType().equals(P)&&m.getName().equals("resetProxyAndRequestList"))methods.add(reset(m));
                else if(c.getType().equals(P)&&SENDS.containsKey(m.getName()))methods.add(send(m));
                else if(c.getType().equals(RESP)&&allowed(m))methods.add(finalHelper(m));
                else if(c.getType().equals(HANDLER)&&m.getName().equals("handleMessage"))methods.add(handler(m));
                else methods.add(m);
            }
            if(c.getType().equals(P)){fields.add(new ImmutableField(P,"mLeaseState",S,AccessFlags.VOLATILE.getValue(),null,Collections.emptySet(),Collections.emptySet()));}
            classes.add(new ImmutableClassDef(c.getType(),c.getAccessFlags(),c.getSuperclass(),c.getInterfaces(),c.getSourceFile(),c.getAnnotations(),fields,methods));
        }
        DexPool.writeTo(args[1],new ImmutableDexFile(source.getOpcodes(),classes));verify(source,read(args[1]),Paths.get(args[2]));
    }
}
